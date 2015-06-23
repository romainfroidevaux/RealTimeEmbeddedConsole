/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract:	CSPI - Configurable Serial Peripheral Interface
 * Author: 	Daniel Gachet / Florian Nicoulaud
 * Date: 	12.02.2014
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cspi.h"
#include "imx27_clock.h"

/* Bit definition and macro CONREG */
#define CSPI_CONREG_BURST 		(1 << 23)
#define CSPI_CONREG_SDHC_SPIEN 		(1 << 22)
#define CSPI_CONREG_SWAP 		(1 << 21)
#define CSPI_CONREG_CS(x)		(((x) & 0x3) << 19)
#define CSPI_CONREG_DATA_RATE(x)	(((x) & 0x1f) << 14)
#define CSPI_CONREG_DR_CTRL(x)		(((x) & 0x3) << 12)
#define CSPI_CONREG_MODE		(1 << 11)
#define CSPI_CONREG_SPIEN		(1 << 10)
#define CSPI_CONREG_XCH			(1 << 9)
#define CSPI_CONREG_SSPOL		(1 << 8)
#define CSPI_CONREG_SSCTL		(1 << 7)
#define CSPI_CONREG_PHA			(1 << 6)
#define CSPI_CONREG_POL			(1 << 5)
#define CSPI_CONREG_BIT_CCOUNT(x)	((x-1) & 0x1f)

/* Bit definition and macro INTREG */
#define CSPI_INTREG_BOEN		(1 << 17)
#define CSPI_INTREG_ROEN		(1 << 16)
#define CSPI_INTREG_RFEN		(1 << 15)
#define CSPI_INTREG_RHEN		(1 << 14)
#define CSPI_INTREG_RREN		(1 << 13)
#define CSPI_INTREG_TSHFEEN		(1 << 12)
#define CSPI_INTREG_TFEN		(1 << 11)
#define CSPI_INTREG_THEN		(1 << 10)
#define CSPI_INTREG_TEEN		(1 << 9)
#define CSPI_INTREG_BO			(1 << 8)
#define CSPI_INTREG_RO			(1 << 7)
#define CSPI_INTREG_RF			(1 << 6)
#define CSPI_INTREG_RH			(1 << 5)
#define CSPI_INTREG_RR			(1 << 4)
#define CSPI_INTREG_TSHFE		(1 << 3)
#define CSPI_INTREG_TF			(1 << 2)
#define CSPI_INTREG_TH			(1 << 1)
#define CSPI_INTREG_TE			(1 << 0)

/* Bit definition and macro PERIODREG */
#define CSPI_PERIODREG_CSRC		(1 << 15)
#define CSPI_PERIODREG_WAIT(x)		((x) & 0x7fff)

/* Bit definition and macro RESETREG */
#define CSPI_RESETREG_START		(1 << 0)


/* SPI2 REGISTER */
struct cspi_regs {
	uint32_t rxdatareg;
	uint32_t txdatareg;	
	uint32_t conreg;
	uint32_t intreg;
	uint32_t testreg;
	uint32_t periodreg;
	uint32_t dmareg;
	uint32_t resetreg;
};

static volatile struct cspi_regs* cspi[] = {
		(struct cspi_regs*) 0x1000e000,
		(struct cspi_regs*) 0x1000f000,
		(struct cspi_regs*) 0x10017000
};

/* clock register */
#define PCCR0_CSPI1_EN (1 << 31);
#define PCCR0_CSPI2_EN (1 << 30);
#define PCCR0_CSPI3_EN (1 << 29);
#define PCCR1_PERCLK2_EN (1 << 9);
static volatile uint32_t* clock_pccr0 = (uint32_t*)0x10027020;
static volatile uint32_t* clock_pcdr1 = (uint32_t*)0x1002701c;
static volatile uint32_t* clock_pccr1 = (uint32_t*)0x10027024;

/* i.MX27 GPIO registers */
struct gpio_ctrl {
	uint32_t ddir;
	uint32_t ocr1;
	uint32_t ocr2;
	uint32_t iconfa1;
	uint32_t iconfa2;
	uint32_t iconfb1;
	uint32_t iconfb2;
	uint32_t dr;
	uint32_t gius;
	uint32_t ssr;
	uint32_t icr1;
	uint32_t icr2;
	uint32_t imr;
	uint32_t isr;
	uint32_t gpr;
	uint32_t swr;
	uint32_t puen;
};

enum gpio_ports {GPIO_PORT_A, GPIO_PORT_B, GPIO_PORT_C,
		 GPIO_PORT_D, GPIO_PORT_E, GPIO_PORT_F,
		 GPIO_NB_OF_PORTS};

static volatile struct gpio_ctrl* gpio[] = {
		(struct gpio_ctrl*)0x10015000, (struct gpio_ctrl*)0x10015100, 
		(struct gpio_ctrl*)0x10015200, (struct gpio_ctrl*)0x10015300, 
		(struct gpio_ctrl*)0x10015400, (struct gpio_ctrl*)0x10015500
};


static inline uint32_t cspi_bit2rate (uint32_t divider)
{
	if (divider <=   3) return 1;
	if (divider <=   4) return 2;
	if (divider <=   6) return 3;
	if (divider <=   8) return 4;
	if (divider <=  12) return 5;
	if (divider <=  16) return 6;
	if (divider <=  24) return 7;
	if (divider <=  32) return 8;
	if (divider <=  48) return 9;
	if (divider <=  64) return 10;
	if (divider <=  96) return 11;
	if (divider <= 128) return 12;
	if (divider <= 192) return 13;
	if (divider <= 256) return 14;
	if (divider <= 384) return 15;
	return 16;
}


static void cspi_write (struct cspi_ctrl* ctrl, 
			uint32_t command, 
			uint32_t data)
{
	volatile struct cspi_regs* spi = ctrl->regs;
	while ((spi->conreg & CSPI_CONREG_XCH) != 0);
	spi->txdatareg = command;
	spi->txdatareg = data;
	spi->conreg |= CSPI_CONREG_XCH;
}


static void cspi_read  (struct cspi_ctrl* ctrl, 
			uint32_t command, 
			uint32_t* data, 
			uint32_t number)
{
	/* make sure than maximum tx buffer will not overflow */
	if (number >= 8) return;
	volatile struct cspi_regs* spi = ctrl->regs;

	/* write command word */
	while ((spi->conreg & CSPI_CONREG_XCH) != 0);
	spi->txdatareg = command;
	/* write clocking words */
	for (uint32_t i = number; i > 0; i--) spi->txdatareg = -1;
	/* start transfer */
	spi->conreg |= CSPI_CONREG_XCH;

	/* read data */
	uint32_t i = 0;
	while ((spi->conreg & CSPI_CONREG_XCH) != 0);
	while ((spi->intreg & CSPI_INTREG_RR)  == 0);
	while ((spi->intreg & CSPI_INTREG_RR)  != 0) {
		uint32_t rx = spi->rxdatareg;
		if ((i > 0) && (i <= number)) *data++ = rx;
		i++;
	}
}


struct cspi_ctrl cspi_init (enum cspi_controllers ctrl, 
			    enum cspi_chipselects cs,
			    uint32_t datarate, 
			    uint32_t wordsize)
{
	/* enable CPSI clock */
	*clock_pcdr1 &= ~(0x3f << 8);
	*clock_pcdr1 |= 1 << 8;
	*clock_pccr1 |= PCCR1_PERCLK2_EN;

	/* configure gpio port D for SPI1 */
	if (ctrl == SPI1) {
		*clock_pccr0 |= PCCR0_CSPI1_EN;
		uint32_t pd = (1<<26)|(1<<27)|(1<<28)|(1<<29)|(1<<30)|(1<<31);
		gpio[GPIO_PORT_D]->gius &= ~pd;
		gpio[GPIO_PORT_D]->gpr  &= ~pd;
	}
	/* configure gpio port D for SPI2 */
	if (ctrl == SPI2) {
		*clock_pccr0 |= PCCR0_CSPI2_EN;
		uint32_t pd = (1<<19)|(1<<20)|(1<<21)|(1<<22)|(1<<23)|(1<<24);
		gpio[GPIO_PORT_D]->gius &= ~pd;
		gpio[GPIO_PORT_D]->gpr  &= ~pd;
	}

	struct cspi_ctrl ctl = {
		.ctrl = ctrl,
		.cs = cs,
		.write = cspi_write,
		.read = cspi_read,
		.regs = cspi[ctrl],
		.private_data = 0,
	};
	volatile struct cspi_regs* spi = ctl.regs;	

	/* reset the CPSI controller */
	spi->resetreg = CSPI_RESETREG_START;
	while ((spi->resetreg & CSPI_RESETREG_START) != 0);

	/* compute data rate */
	uint32_t divider = imx27_clock_get_perclk2() / datarate / 1000;
	uint32_t bitrate = cspi_bit2rate (divider);

	spi->conreg =  CSPI_CONREG_BURST
		    | CSPI_CONREG_CS(cs)
		    | CSPI_CONREG_DATA_RATE(bitrate)
		    | (cs != NO_CS ? CSPI_CONREG_MODE : 0)
		    | CSPI_CONREG_SPIEN
		    | CSPI_CONREG_PHA
		    | CSPI_CONREG_BIT_CCOUNT(wordsize);

	spi->intreg = 0;
	spi->testreg = 0;
	spi->periodreg = 0;
	spi->dmareg = 0;

	return ctl;
}

