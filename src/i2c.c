/**
 * EIA-FR - Embedded Systems 1 laboratory
 *
 * Abstract: 	I2C Device Driver
 *
 * Purpose:	This module implements the services for the I2C module
 *          	of the i.MX27 processor.
 *
 * Author:	Daniel Gachet / EIA-FR / Telecoms
 * Date:	28.11.2013
 */

#include <stdbool.h>
#include <stdio.h>

#include "imx27_clock.h"
#include "i2c.h"

/* -- Internal types and constant definition -------------------------------- */

/* I2C Clock rate conversion table */
struct ic_pair {uint16_t divider; uint16_t ic; };
static const struct ic_pair ic_divider[] = {
	{  22, 0x20}, {  24, 0x21}, {  26, 0x22}, {  28, 0x23},
	{  30, 0x00}, {  32, 0x01}, {  36, 0x02}, {  40, 0x26},
	{  42, 0x03}, {  44, 0x27}, {  48, 0x04}, {  52, 0x05},
	{  56, 0x29}, {  60, 0x06}, {  64, 0x2a}, {  72, 0x07},
	{  80, 0x08}, {  88, 0x09}, {  96, 0x2d}, { 104, 0x0a},
	{ 112, 0x2e}, { 128, 0x0b}, { 144, 0x0c}, { 160, 0x0d},
	{ 192, 0x0e}, { 224, 0x32}, { 240, 0x0f}, { 256, 0x33},
	{ 288, 0x10}, { 320, 0x11}, { 384, 0x12}, { 448, 0x36},
	{ 480, 0x13}, { 512, 0x37}, { 576, 0x14}, { 640, 0x15},
	{ 768, 0x16}, { 896, 0x3a}, { 960, 0x17}, {1024, 0x3b},
	{1152, 0x18}, {1280, 0x19}, {1536, 0x1a}, {1792, 0x3e},
	{1920, 0x1b}, {2048, 0x3f}, {2304, 0x1c}, {2560, 0x1d},
	{3072, 0x1e}, {3840, 0x1f}
};
static const int ic_pair_len = (sizeof(ic_divider) / sizeof(struct ic_pair));

/* I2C Control Register (I2CR) bit fields */
#define I2CR_IEN 	(1<<7)	/* I2C enable */
#define I2CR_IIEN 	(1<<6)	/* I2C interrupt enable */
#define I2CR_MSTA	(1<<5)	/* Master/slave mode select bit */
#define I2CR_MTX	(1<<4)	/* Transmit/receive mode select bit */
#define I2CR_TXAK	(1<<3)	/* Transmit acknowledge enable */
#define I2CR_RSATA	(1<<2)	/* Repeat start */

/* I2C Status Register (I2SR) bit fields */
#define I2SR_ICF	(1<<7)	/* Data transferring */
#define I2SR_IAAS	(1<<6)	/* I2C addressed as a slave */
#define I2SR_IBB	(1<<5)	/* I2C bus busy */
#define I2SR_IAL	(1<<4)	/* Arbitration lost */
#define I2SR_SRW	(1<<2)	/* Slave read/write */
#define I2SR_IIF	(1<<1)	/* I2C interrupt */
#define I2SR_RXAK	(1<<0)	/* Received acknowledge */

/* definition of I2C operations within address word */
#define I2C_READ_OPER 	1
#define I2C_WRITE_OPER 	0


/* I2C Controller Register Definition */
struct i2c_ctrl {
	uint32_t iadr;
	uint32_t ifdr;
	uint32_t i2cr;
	uint32_t i2sr;
	uint32_t i2dr;
};
static volatile struct i2c_ctrl* i2c = (struct i2c_ctrl*)0x10012000;



/* -- Internal methods definition ------------------------------------------- */

/**
 * generate a start bit as master on the i2c bus
 */
static void i2c_send_start()
{
	while ((i2c->i2sr & I2SR_IBB) != 0);	// wait until line is ready
	i2c->i2cr |= I2CR_MSTA; 		// generate a start bit
}

/**
 * generate a stop bit as master on the i2c bus
 */
static void i2c_send_stop()
{
	while ((i2c->i2sr & I2SR_ICF) == 0);	// wait until transfer has completed
	i2c->i2cr &= ~I2CR_MSTA; 		// generate stop bit
}

/**
 * generate a repeated start bit as master on the i2c bus
 */
static void i2c_send_repeated_start()
{
	while ((i2c->i2sr & I2SR_ICF) == 0);	// wait until transfer has completed
	i2c->i2cr |= I2CR_RSATA; 		// generate a restart bit
}

/**
 * switch master from writing to reading mode
 */
static void i2c_switch_to_read_operation (bool ackn)
{
	while ((i2c->i2sr & I2SR_ICF) == 0);	// wait until transfer as completed
	i2c->i2cr &= ~I2CR_MTX;			// according to chap. 24.5.3, switch

	if (ackn) i2c->i2cr &= ~I2CR_TXAK;	// prepare acknowledgement for next 
	else	  i2c->i2cr |=  I2CR_TXAK;	// data byte reading as required

	int data __attribute__((unused)) = i2c->i2dr;	// idr2 [r/w] & i2cr[mtx]
}

/**
 * send a 8-bit data on the i2c bus and wait for completion
 */
static int i2c_write_byte (uint8_t data)
{
	i2c->i2cr |= I2CR_MTX;			// signal start of data transmission
	i2c->i2dr = data;			// send data on the bus

	while ((i2c->i2sr & I2SR_ICF) == 0);	// wait until transfer has completed
	while ((i2c->i2sr & I2SR_IIF) == 0);	// according to chap. 24.5.3, check i2sr[iif]
	i2c->i2sr &= ~I2SR_IIF;			// and clear it

	return (i2c->i2sr & I2SR_RXAK) != 0 ? -1 : 0;	// return -1 if no acknowledge received
}

/**
 * read a 8-bit character from the i2c bus
 */
static uint8_t i2c_read_byte (bool ackn)
{
	while ((i2c->i2sr & I2SR_ICF) == 0); 	// wait until transfer as completed
	while ((i2c->i2sr & I2SR_IIF) == 0);	// according to chap. 24.5.3, check i2sr[iif]
	i2c->i2sr &= ~I2SR_IIF;			// and clear it

	if (ackn) i2c->i2cr &= ~I2CR_TXAK;	// prepare acknowledgement for next 
	else	  i2c->i2cr |=  I2CR_TXAK;	// data byte reading as required

	short  data = i2c->i2dr;		// return received data
	return data;
}



/* -- Public methods definition --------------------------------------------- */

void i2c_init ()
{
	printf("Init: i2c\n\r");

	imx27_clock_enable_line (IMX27_CLOCK_I2C1);

	i2c->i2cr = 0;
	i2c_set_bus_speed(100000);

	i2c->iadr = 9<<1; /* slave #9 */

	i2c->i2cr = I2CR_IEN;
	i2c->i2sr = 0;
}

/* ------------------------------------------------------------------------- */

void i2c_set_bus_speed (uint32_t speed)
{
	uint32_t freq1 = imx27_clock_get_ahbclk() / 2;
	uint32_t refdiv = freq1 / speed;

	/* binary search with ic_divider table */
	int l = -1;
	int r = ic_pair_len - 1;
	while ((r - l) > 1) {
		int m = (l + r) / 2;
		if (ic_divider[m].divider < refdiv) {
            		l = m;
		} else {
			r = m;
		}
	}

	i2c->ifdr = ic_divider[r].ic;
}

/* ------------------------------------------------------------------------- */

int i2c_read (uint8_t chip_id, uint8_t reg_addr, uint8_t* buffer, uint16_t len)
{
	int status = 0;

	/* send chip internal register address (pointer register) */
	if (status == 0) i2c_send_start ();
	if (status == 0) status = i2c_write_byte (chip_id << 1 | I2C_WRITE_OPER);
	if (status == 0) status = i2c_write_byte (reg_addr);
	if (status == 0) i2c_send_repeated_start ();

	/* send again chip id before to switch to read mode */
	if (status == 0) status = i2c_write_byte ((chip_id << 1) | I2C_READ_OPER);
	if (status == 0) i2c_switch_to_read_operation(len > 1);

	/* read specified number of bytes */
	if (status == 0) {
		while (len > 1) {
			*buffer++ = i2c_read_byte (--len > 1);
		}
	}
	i2c_send_stop ();
	if (status == 0) *buffer++ = i2c_read_byte (false);

	return status;
}

/* ------------------------------------------------------------------------- */

int i2c_write (uint8_t chip_id, uint8_t reg_addr, const uint8_t* buffer, uint16_t len)
{
	int status = 0;

	/* send 1st chip internal register address (pointer register) */
	if (status == 0) i2c_send_start ();
	if (status == 0) status = i2c_write_byte (chip_id << 1 | I2C_WRITE_OPER);
	if (status == 0) status = i2c_write_byte (reg_addr);

	/* write next the buffer into the chip */
	while ((status == 0) && (len > 0)) {
		len--;
		status = i2c_write_byte (*buffer++);
	}
	i2c_send_stop ();

	return status;
}

/* ------------------------------------------------------------------------- */

bool i2c_probe (uint8_t chip_id)
{
	i2c_send_start ();
	int status = i2c_write_byte ((chip_id << 1) | I2C_WRITE_OPER);
	i2c_send_stop ();

	return status == 0;
}

