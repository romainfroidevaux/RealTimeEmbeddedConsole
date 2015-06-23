/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract:	FEC - Fast Ethernet Controller
 *
 * Purpose:	Module to deal with the Fast Ethernet Controller 
 * 		(FEC) of the i.MX27.
 *
 * Updates : Show Mac address on startup
 *
 * Author: 	Daniel Gachet / Alexandre Lefranc / David Rossier
 * Date: 	29.04.2014
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <imx27_clock.h>

#include "fec.h"

/** -------------------------------------------------------------------------
 * Macros
 * ------------------------------------------------------------------------- */

#define MIN(x, y) ((x < y) ? x : y)

#define PRINTD printf


/** -------------------------------------------------------------------------
 * utilities
 * ------------------------------------------------------------------------- */

// written in assembleur to avoid compiler optimization...
static void usleep(uint32_t timeout __attribute__((unused)))
{
	__asm__("   adds  r0, #1;"
		"   subeq r0, #1;"
		"1: nop;"
		"   nop;"
		"   subs  r0, #1;"
		"   bne   1b;"
	       );
}



/** -------------------------------------------------------------------------
 * i.MX27 Clocking Registers
 * ------------------------------------------------------------------------- */

#define PCCR0_FEC_EN            (1<<26)
#define PCCR1_HCLK_FEC          (1<<17)

static volatile uint32_t* pccr0 = (uint32_t *)0x10027020;
static volatile uint32_t* pccr1 = (uint32_t *)0x10027024;


/** -------------------------------------------------------------------------
 * Fast Ethernet Controller register definition
 * ------------------------------------------------------------------------- */

/* Ethernet Interrupt Event Register */
#define FEC_EIR_HBERR	(1<<31)
#define FEC_EIR_BABR	(1<<30)
#define FEC_EIR_BABT	(1<<29)
#define FEC_EIR_GRA	(1<<28)
#define FEC_EIR_TXF	(1<<27)
#define FEC_EIR_TXB	(1<<26)
#define FEC_EIR_RXF	(1<<25)
#define FEC_EIR_RXB	(1<<24)
#define FEC_EIR_MII	(1<<23)
#define FEC_EIR_EBERR	(1<<22)
#define FEC_EIR_LC	(1<<21)
#define FEC_EIR_RL	(1<<20)
#define FEC_EIR_UN	(1<<19)

#define FEC_EIR_TX_ERR	(FEC_EIR_BABT | FEC_EIR_LC | FEC_EIR_RL | FEC_EIR_UN)
#define FEC_EIR_RX_ERR	(FEC_EIR_BABR)
#define FEC_EIR_ERR	(FEC_EIR_HBERR | FEC_EIR_EBERR)

/* Ethernet Interrupt Mask Register */
#define FEC_EIMR_HBERR	(1<<31)
#define FEC_EIMR_BABR	(1<<30)
#define FEC_EIMR_BABT	(1<<29)
#define FEC_EIMR_GRA	(1<<28)
#define FEC_EIMR_TXF	(1<<27)
#define FEC_EIMR_TXB	(1<<26)
#define FEC_EIMR_RXF	(1<<25)
#define FEC_EIMR_RXB	(1<<24)
#define FEC_EIMR_MII	(1<<23)
#define FEC_EIMR_EBERR	(1<<22)
#define FEC_EIMR_LC	(1<<21)
#define FEC_EIMR_RL	(1<<20)
#define FEC_EIMR_UN	(1<<19)

/* Receive Descriptor Active Register */
#define FEC_RDAR_RX_ACTIVE (1<<24)

/* Transmit Descriptor Active Register */
#define FEC_TDAR_TX_ACTIVE (1<<24)

/* Ethernet Control Register */
#define FEC_ECR_RESET	(1<<0)
#define FEC_ECR_ETHER_EN (1<<1)

/* MIB Control Register */
#define FEC_MIB_DISABLE	(1<<31)

/* Transmit Control Register */
#define FEC_TCR_FDEN	(1<<2)
#define FEC_TCR_HBC	(1<<1)

/* Receive Control Register */
#define FEC_RCR_MII_MODE (1<<2)
#define FEC_RCR_PROM	(1<<3)
#define FEC_RCR_BC_REJ	(1<<4)
#define FEC_RCR_FCE	(1<<5)
#define FEC_RCR_MAX_FL	(1518 << 16)

/* MII */
#define FEC_MII_ST	0x40000000
#define FEC_MII_OP_OFF	28
#define FEC_MII_OP_MASK	0x03
#define FEC_MII_OP_RD	0x02
#define FEC_MII_OP_WR	0x01
#define FEC_MII_PA_OFF	23
#define FEC_MII_PA_MASK	0xFF
#define FEC_MII_RA_OFF	18
#define FEC_MII_RA_MASK	0xFF
#define FEC_MII_TA	0x00020000
#define FEC_MII_DATA_OFF  0
#define FEC_MII_DATA_MASK 0x0000FFFF

#define FEC_MII_FRAME		(FEC_MII_ST | FEC_MII_TA)
#define FEC_MII_OP(x)		(((x) & FEC_MII_OP_MASK) << FEC_MII_OP_OFF)
#define FEC_MII_PA(pa)		(((pa) & FEC_MII_PA_MASK) << FEC_MII_PA_OFF)
#define FEC_MII_RA(ra)		(((ra) & FEC_MII_RA_MASK) << FEC_MII_RA_OFF)
#define FEC_MII_SET_DATA(v)	(((v) & FEC_MII_DATA_MASK) << FEC_MII_DATA_OFF)
#define FEC_MII_GET_DATA(v)	(((v) >> FEC_MII_DATA_OFF) & FEC_MII_DATA_MASK)

#define FEC_MII_READ(pa,ra)	((FEC_MII_FRAME | FEC_MII_OP(FEC_MII_OP_RD))|\
					FEC_MII_PA(pa) | FEC_MII_RA(ra))

#define FEC_MII_WRITE(pa,ra,v)	(FEC_MII_FRAME | FEC_MII_OP(FEC_MII_OP_WR)|\
					FEC_MII_PA(pa) | FEC_MII_RA(ra) | \
					FEC_MII_SET_DATA(v))

/* FEC Registers */
struct fec_ctrl {
	uint32_t res0[1];	/* Reserved */
	uint32_t eir;		/* Ethernet Interrupt Event Register */
	uint32_t eimr;		/* Ethernet Interrupt Mask Register */
	uint32_t res1[1];	/* Reserved */
	uint32_t rdar;		/* Receive Descriptor Active Register */
	uint32_t tdar;		/* Transmit Descriptor Active Register */
	uint32_t res2[3];	/* Reserved */
	uint32_t ecr;		/* Ethernet Control Register */
	uint32_t res3[6];	/* Reserved */
	uint32_t mmfr;		/* MII Management Frame Register */
	uint32_t mscr;		/* MII Speed Control Register */
	uint32_t res4[7];	/* Reserved */
	uint32_t mibc;		/* MIB Control Register */
	uint32_t res5[7];	/* Reserved */
	uint32_t rcr;		/* Receive Control Register */
	uint32_t res6[15];	/* Reserved */
	uint32_t tcr;		/* Transmit Control Register */
	uint32_t res7[7];	/* Reserved */
	uint32_t palr;		/* Physical Address Low Register */
	uint32_t paur;		/* Physical Address High + Type Register */
	uint32_t opd;		/* Opcode/Pause Duration Register */
	uint32_t res8[10];	/* Reserved */
	uint32_t iaur;		/* Descr. Indiv. upper addr reg (hash table) */
	uint32_t ialr;		/* Descr. Indiv. lower addr reg (hash table) */
	uint32_t gaur;		/* Descr. Group upper addr reg (hash table) */
	uint32_t galr;		/* Descr. Group lower addr reg (hash table) */
	uint32_t res9[7];	/* Reserved */
	uint32_t tfwr;		/* Transmit FIFO Watermark Register */
	uint32_t res10[1];	/* Reserved */
	uint32_t frbr;		/* FIFO Receive Bound Register */
	uint32_t frsr;		/* FIFO Receive FIFO Start Register */
	uint32_t res11[11];	/* Reserved */
	uint32_t erdsr;		/* Receive Buffer Descr. Ring Start Register */
	uint32_t etdsr;		/* Transmit Buffer Descr. Ring Start Register */
	uint32_t emrbr;		/* Receive Buffer Size Register */
};
static volatile struct fec_ctrl* fec = (struct fec_ctrl*)0x1002b000;


/**
 * FEC SNMP MIB Counters
 */
#define FEC_MIB_BASE            0x1002b200      /* Counter base address */
#define FEC_MIB_SIZE            0x200           /* Counter size */

/**
 * FEC Buffer Descriptor Definition 
 */
/* Ethernet Receive Buffer Descriptor */
#define FEC_BD_RX_EMPTY		(1<<15)
#define FEC_BD_RX_WRAP		(1<<13)
#define FEC_BD_RX_LAST		(1<<11)
#define FEC_BD_RX_M		(1<<8)
#define FEC_BD_RX_BC		(1<<7)
#define FEC_BD_RX_MC		(1<<6)
#define FEC_BD_RX_LG		(1<<5)
#define FEC_BD_RX_NO		(1<<4)
#define FEC_BD_RX_CR		(1<<2)
#define FEC_BD_RX_OV		(1<<1)
#define FEC_BD_RX_TR		(1<<0)
#define FEC_BD_RX_ERRS		(FEC_BD_RX_LG | FEC_BD_RX_NO | FEC_BD_RX_CR | \
				 FEC_BD_RX_OV | FEC_BD_RX_TR)

/* Ethernet Transmit Buffer Descriptor */
#define FEC_BD_TX_READY		(1<<15)
#define FEC_BD_TX_WRAP		(1<<13)
#define FEC_BD_TX_LAST		(1<<11)
#define FEC_BD_TX_TC		(1<<10)
#define FEC_BD_TX_ABC		(1<<9)

/* Buffer Descriptor */
struct fec_bd {
        uint16_t length;        /* Data length */
        uint16_t status;        /* Status of this buffer */
        uint32_t data;          /* Data Buffer Pointer */
};

/* Buffer dimensioning  */
#define FEC_BD_RX_NUM           64
#define FEC_BD_TX_NUM           2
#define FEC_PACKET_SIZE         1536
#define DB_ALIGNMENT            16
#define DB_DATA_ALIGNMENT       16

static volatile struct fec_bd* rx_bd = (struct fec_bd*)0xb0800000;
static volatile struct fec_bd* tx_bd = (struct fec_bd*)0xb0c00000;
static uint8_t* rx_buff = (uint8_t*)0xb1000000;
static uint8_t* tx_buff = (uint8_t*)0xb2000000;
static uint32_t rx_bd_index;
static uint32_t tx_bd_index;

/**
 *  Allocation and initialization of rx & RxBD/TxBD rings.
 */
static void fec_bd_init ()
{
	/* Clear rx buffers memory */
        memset(rx_buff, 0, FEC_BD_RX_NUM * FEC_PACKET_SIZE);

        /* Initialize rx buffer descriptors */
        for (int i = 0; i < FEC_BD_RX_NUM; i++) {
                rx_bd[i].status = FEC_BD_RX_EMPTY;
                rx_bd[i].length = 0;
                rx_bd[i].data = ((uint32_t) rx_buff) + (FEC_PACKET_SIZE * i);
        }
        rx_bd[FEC_BD_RX_NUM-1].status |= FEC_BD_RX_WRAP; /* mark last bd */

        /* Initialize tx buffer descriptors */
        for (int i = 0; i < FEC_BD_TX_NUM; i++) {
                tx_bd[i].status = 0;
                tx_bd[i].length = 0;
                tx_bd[i].data = 0;
        }
        tx_bd[FEC_BD_TX_NUM - 1].status |= FEC_BD_TX_WRAP; /* mark last bd */

	/* Initialize indexes */
        rx_bd_index = 0;
        tx_bd_index = 0;
}


/** -------------------------------------------------------------------------
 * MII Interface
 * ------------------------------------------------------------------------- */

/** 
 * LAN8700 PHY Chip register definition 
 */
/* MII registers */
#define MII_BMCR		0x00	/* Basic Control Register	*/
#define MII_BMSR		0x01	/* Basic Status Register	*/
#define MII_PHYSID1		0x02	/* PHYS ID 1			*/
#define MII_PHYSID2		0x03	/* PHYS ID 2			*/
#define MII_ADVERTISE		0x04	/* Advertisement Control Reg	*/
#define MII_LPA			0x05	/* Link Partner Ability Reg	*/
#define MII_EXPANSION		0x06	/* Expansion Register		*/
#define MII_MCS			0x11	/* Mode Control/Status		*/
#define MII_SPECIAL_REG		0x12	/* Special Register		*/
#define MII_INT_SRC		0x1D	/* Interrupt Source Flags	*/

/* Basic mode control register */
#define BMCR_CTST		(1<<7)	/* Collision test		*/
#define BMCR_FULLDPLX		(1<<8)	/* Full duplex			*/
#define BMCR_ANRESTART		(1<<9)	/* Auto negotiation restart	*/
#define BMCR_ISOLATE		(1<<10)	/* Electrical isolation of PHY	*/
#define BMCR_PDOWN		(1<<11)	/* Powerdown			*/
#define BMCR_ANENABLE		(1<<12)	/* Enable auto negotiation	*/
#define BMCR_SPEED100		(1<<13)	/* 100Mbps			*/
#define BMCR_LOOPBACK		(1<<14)	/* Loopback			*/
#define BMCR_RESET		(1<<15)	/* Reset			*/

/* Basic mode status register */
#define BMSR_ERCAP		0x0001	/* Ext-reg capability           */
#define BMSR_JCD		0x0002	/* Jabber detected              */
#define BMSR_LSTATUS		0x0004	/* Link status                  */
#define BMSR_ANEGCAPABLE	0x0008	/* Able to do auto-negotiation  */
#define BMSR_RFAULT		0x0010	/* Remote fault detected        */
#define BMSR_ANEGCOMPLETE	0x0020	/* Auto-negotiation complete    */
#define BMSR_10HALF		0x0800	/* Can do 10Mbps, half-duplex   */
#define BMSR_10FULL		0x1000	/* Can do 10Mbps, full-duplex   */
#define BMSR_100HALF		0x2000	/* Can do 100Mbps, half-duplex  */
#define BMSR_100FULL		0x4000	/* Can do 100Mbps, full-duplex  */
#define BMSR_100BASE4		0x8000	/* Can do 100Mbps, 4k packets   */

/* Advertisement control register */
#define ADVERTISE_SLCT		0x001f	/* Selector bits                */
#define ADVERTISE_CSMA		0x0001	/* Only selector supported      */
#define ADVERTISE_10HALF	0x0020	/* Try for 10Mbps half-duplex   */
#define ADVERTISE_10FULL	0x0040	/* Try for 10Mbps full-duplex   */
#define ADVERTISE_100HALF	0x0080	/* Try for 100Mbps half-duplex  */
#define ADVERTISE_100FULL	0x0100	/* Try for 100Mbps full-duplex  */
#define ADVERTISE_100BASE4	0x0200	/* Try for 100Mbps 4k packets   */
#define ADVERTISE_RESV		0x1c00	/* Unused...                    */
#define ADVERTISE_RFAULT	0x2000	/* Say we can detect faults     */
#define ADVERTISE_LPACK		0x4000	/* Ack link partners response   */
#define ADVERTISE_NPAGE		0x8000	/* Next page bit                */

/* Link partner ability register */
#define LPA_SLCT		0x001f	/* Same as advertise selector   */
#define LPA_10HALF		0x0020	/* Can do 10Mbps half-duplex    */
#define LPA_10FULL		0x0040	/* Can do 10Mbps full-duplex    */
#define LPA_100HALF		0x0080	/* Can do 100Mbps half-duplex   */
#define LPA_100FULL		0x0100	/* Can do 100Mbps full-duplex   */
#define LPA_100BASE4		0x0200	/* Can do 100Mbps 4k packets    */
#define LPA_RESV		0x1c00	/* Unused...                    */
#define LPA_RFAULT		0x2000	/* Link partner faulted         */
#define LPA_LPACK		0x4000	/* Link partner acked us        */
#define LPA_NPAGE		0x8000	/* Next page bit                */
#define LPA_DUPLEX		(LPA_10FULL | LPA_100FULL)
#define LPA_100			(LPA_100FULL | LPA_100HALF | LPA_100BASE4)

/* Interrupt Sources Flags */
#define INT_ENERGYON		(1<<7)	/* ENERGYON generated		*/
#define INT_AUTO_NEG		(1<<6)	/* Auto-Negotiation complete	*/
#define INT_RFAULT		(1<<5)	/* Remote Fault Detected	*/
#define INT_LINK_DWN		(1<<4)	/* Link Down			*/
#define INT_ANEG_LP_ACK		(1<<3)	/* Auto-Negotiation LP ACK	*/
#define INT_PARALLEL_FAULT	(1<<2)	/* Parallel Detection Fault	*/
#define INT_ANED_PAGE_RCV	(1<<1)	/* Auto-Negotation Page Received*/

/* MII PHY Address */
#define MII_PHY_ADDR            0x1F

#define TIMEOUT (1000*20)
#define TIMEOUT_LONG (100000*20)

/**
 *  MII read operation
 * 
 * @param reg_addr remote device register address
 * @param value read register value
 * @return status, 0 for success, -1 for error
 */
static int fec_mii_read (uint8_t reg_addr, uint16_t * value)
{
	int status = -1;

	fec->mmfr = FEC_MII_READ(MII_PHY_ADDR, reg_addr);
	
	long timeout = TIMEOUT;
	while ((timeout > 0) && (status != 0)) {
		usleep (100); /* short delay */
		if ((fec->eir & FEC_EIR_MII) != 0) {
			fec->eir = FEC_EIR_MII;
			*value = FEC_MII_GET_DATA(fec->mmfr);
			status = 0;
		}
		timeout--;
	}
	if (status != 0) PRINTD("[MII] fec_mii_read: timeout\n");

	return status;
}

/**
 *  MII write operation
 *
 * @param reg_addr remote device register address
 * @param value register value to write
 * @return status, 0 for success, -1 for error
 */
static int fec_mii_write (uint8_t reg_addr, uint16_t value)
{
	int status = -1;

	fec->mmfr = FEC_MII_WRITE(MII_PHY_ADDR, reg_addr, value);

	long timeout = TIMEOUT;
	while ((timeout > 0) && (status != 0)) {
		usleep(100); /* short delay */
		if ((fec->eir & FEC_EIR_MII) != 0) {
			fec->eir = FEC_EIR_MII;
			status = 0;
		}
		timeout--;
	}
	if (status != 0) PRINTD("[MII] fec_mii_write: timeout\n");

	return status;
}


/**
 *  MII contoller initialization
 *
 * @return status, 0 for success, -1 for error
 */
int fec_mii_init ()
{
	int status = -1;
	char* error_string="";

	/* Set mode = 111. All capable. Auto-negotiation enabled. */
	fec_mii_write(MII_SPECIAL_REG, 0x00ff);

	/* Reset */
	fec_mii_write(MII_BMCR, BMCR_RESET);
	usleep(TIMEOUT_LONG); /* delay */

	/* Auto-negotiation */
	fec_mii_write(MII_BMCR, BMCR_ANENABLE | BMCR_ANRESTART);

	/* Wait for auto-negotiation complete */
	error_string = "[MII] Auto-Negotiation Timeout\n";
	int timeout = 500;
	uint16_t intsrc = 0;
	while ((timeout > 0) && ((intsrc & INT_AUTO_NEG) == 0)) {
		usleep (TIMEOUT_LONG); /* delay */
		fec_mii_read(MII_INT_SRC, &intsrc);
		timeout--;
	}
	
	char* state="";
	char* duplex="";
	char* speed="";
	if (timeout > 0) {
		error_string = "[MII] Link Status Timeout\n";
		timeout = 500;
		uint16_t bmsr = 0;
		while ((timeout > 0) && ((bmsr & BMSR_LSTATUS) == 0)) {
			usleep(TIMEOUT_LONG); /* delay */
			fec_mii_read(MII_BMSR, &bmsr);
			timeout--;
		}
		state = (bmsr & BMSR_LSTATUS) ? "up" : "down";
	}

	if (timeout > 0) {
		uint16_t lpa = 0;
		uint16_t bmcr = 0;
		fec_mii_read(MII_BMCR, &bmcr);
		fec_mii_read(MII_LPA, &lpa);

		if ((bmcr & BMCR_ANENABLE) != 0) {
			duplex = (lpa & LPA_DUPLEX) != 0 ? "Full" : "Half";
			speed  = (lpa & LPA_100   ) != 0 ? "100"  : "10";
		} else {
			duplex = (bmcr & BMCR_FULLDPLX) != 0 ? "Full" : "Half";
			speed  = (bmcr & BMCR_SPEED100) != 0 ? "100"  : "10";
		}

		PRINTD("[MII] Link is %s - %s Mbps/%s-duplex\n", 
			state, speed, duplex);

		status = 0;
	}

	if (status != 0) PRINTD (error_string);

	return status;
}

/** -------------------------------------------------------------------------
 * IIM Interface
 * ------------------------------------------------------------------------- */

/**
 *  Get MAC address from the flash.
 */
void fec_get_mac_address (uint8_t* mac)
{
	static volatile uint32_t* iim_mac = (uint32_t*)0x10028814;

	/* Get physical address */
	for (int i = 0; i < 6; i++) mac[5 - i] = iim_mac[i];
}

/**
 *  Set MAC address to the register.
 */
void fec_set_mac_address (const uint8_t* mac)
{
	/* Set physical address */
	fec->palr = (mac[0] << 24) + (mac[1] << 16) + (mac[2] <<  8) + mac[3];
	fec->paur = (mac[4] << 24) + (mac[5] << 16) + 0x8808;

//	PRINTD("[ETHER] MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
//	    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}


/** -------------------------------------------------------------------------
 * FEC Interface
 * ------------------------------------------------------------------------- */

/**
 *  FEC Initialization.
 */
int fec_init ()
{
	//PRINTD("[FEC] Initialization\n");

	/* Set clocks */
	*pccr0 |= PCCR0_FEC_EN;
	*pccr1 |= PCCR1_HCLK_FEC;

	/* Reset the FEC */
	fec->ecr = FEC_ECR_RESET;
	while ((fec->ecr & FEC_ECR_RESET) != 0);

	/* Set interrupt mask register and clear pending interrupts */
	fec->eimr = 0;
	fec->eir = -1;

	/* Disable MIB counters and clear all the counters */
	fec->mibc |= FEC_MIB_DISABLE;
	uint32_t* mib = (uint32_t*)FEC_MIB_BASE;
	for (int i = FEC_MIB_SIZE/4; i > 0; i--) *mib++ = 0;

	/* Set Watermark register 192 bytes */
	fec->tfwr = 0x3;

	/* Set individual address register */
	fec->ialr = 0;
	fec->iaur = 0;

	/* Set multicast address filter */
	fec->galr = 0;
	fec->gaur = 0;

	/* Get & Set MAC Address */
	uint8_t mac_addr[6];
	fec_get_mac_address(mac_addr);
	fec_set_mac_address(mac_addr);

	/* Set Opcode/Pause Duration Register */
	fec->opd = 0x00010020;

	/*
	 * Set Receive Control Register
	 * Frame length = 1518; Flow control enable; Promiscuous mode; MII mode
	 */
	fec->rcr=FEC_RCR_MAX_FL | FEC_RCR_FCE | FEC_RCR_PROM | FEC_RCR_MII_MODE;

	/*
	 * Set Transmit Control Register
	 * Full-Duplex enable
	 */
	fec->tcr = FEC_TCR_FDEN;

	/* Set MII speed */
	uint32_t ahbclk = imx27_clock_get_ahbclk();
	fec->mscr = (((ahbclk / 1000000) + 2) / 5) << 1;

	/* Set FIFO receive FIFO start register */
	fec->frsr = 0x520;

	/* Set buffer size register */
	fec->emrbr = FEC_PACKET_SIZE;

	/* Initialize BD rings */
	fec_bd_init();

	/* Set Rx & Tx buffers descriptors rings start registers */
	fec->erdsr = (uint32_t) rx_bd;
	fec->etdsr = (uint32_t) tx_bd;

	return 0;
}


int fec_start (bool promiscuous, bool broadcast)
{
        if (promiscuous) fec->rcr |= FEC_RCR_PROM;
                    else fec->rcr &= ~FEC_RCR_PROM;

        if (broadcast) fec->rcr &= ~FEC_RCR_BC_REJ;
                  else fec->rcr |= FEC_RCR_BC_REJ;

	/* Enable the FEC */
	fec->ecr |= FEC_ECR_ETHER_EN;

	/* Start Auto-Negotiation */
	int status = fec_mii_init();

	if (status == 0) {
		/* Activate rx */
		fec->rdar |= FEC_RDAR_RX_ACTIVE;
		PRINTD("[FEC] %s\n", ((fec->ecr & FEC_ECR_ETHER_EN) != 0) 
			? "Running" : "Stopped");
		uint8_t mac[6];
		fec_get_mac_address(mac);
		/*
		 * Print mac address.
		 */
		PRINTD("[ETHER] MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}

	return status;
}


int fec_stop ()
{
	/* Disable the FEC */
	fec->ecr &= ~FEC_ECR_ETHER_EN;
	usleep(TIMEOUT_LONG); /* delay */

	/* Initialize BD rings */
	fec_bd_init();

	PRINTD("[FEC] Stopped\n");

	return 0;
}

/**
 *  FEC receive.
 */
int fec_recv (void* buffer, size_t length)
{
	size_t frame_length = 0;

	/* check if fec is running */	
	if ((fec->ecr & FEC_ECR_ETHER_EN) == 0) {
//		PRINTD("[!] FEC controller disabled\n");
		return -1;
	}

	/* Check arguments */
	if ((buffer == NULL) || (length <= 0)) {
		PRINTD("[!] fec_rcv: argument error\n");
		return -1;
	}

	/* Get and acknowledge events */
	uint32_t ievent = fec->eir;
	fec->eir = ievent;

	/* Check events */
	if (ievent == 0) return 0;

	if ((ievent & FEC_EIR_BABR) != 0) {
		PRINTD("[!] fec_rcv: Babbling receive error (eir = %lx)\n", 
			ievent);
		return -1;
	}

	if ((ievent & FEC_EIR_HBERR) != 0)
		fec->tcr |= FEC_TCR_HBC;

	if ((ievent & FEC_EIR_GRA) != 0) {
		if (fec->tcr & FEC_TCR_HBC) {
			/*
			fec_stop(fec);
			fec->tcr &= ~FEC_TCR_HBC;
			fec_init(fec);
			*/
		}
	}

	if ((ievent & FEC_EIR_EBERR) != 0) {
		PRINTD("[!] fec_rcv: Ethernet Bus Error!\n");
		return -1;
	}

	/* Read current receive buffer descriptor */
        volatile struct fec_bd *rbd = &rx_bd[rx_bd_index];

	/* Get status */
	uint16_t status = rbd->status;

	/* Check status */
	if ((status & FEC_BD_RX_EMPTY) == 0) {
		/* Get data length */
		frame_length = rbd->length - 4;

		/* Get data pointer */
		uint8_t* frame = (uint8_t *) rbd->data;

		if (((status & FEC_BD_RX_LAST) != 0)
			&& ((status & FEC_BD_RX_ERRS) == 0)
			&& (frame_length > 14))
		{
			//net_receive(rx_buffer, frame_length);
			memcpy(buffer, frame, MIN(frame_length, length));
		}
		else
		{
			if ((status & FEC_BD_RX_ERRS) != 0)
				PRINTD("[!] fec_rcv: error frame: 0x%x\n", 
					status);
		}

		/* Clear descriptor */
		rbd->status = (status & FEC_BD_RX_WRAP) | FEC_BD_RX_EMPTY;
		rbd->length = 0;

		/* Set next receive descriptor */
		rx_bd_index = (rbd->status & FEC_BD_RX_WRAP) != 0 ? 0 : 
				rx_bd_index + 1;

		/* Rx active */
		fec->rdar |= FEC_RDAR_RX_ACTIVE;
	}

	return frame_length;
}


/**
 *  FEC send.
 */
int fec_send (const void * frame, size_t length)
{
	/* check if fec is running */	
	if ((fec->ecr & FEC_ECR_ETHER_EN) == 0) {
//		PRINTD("[!] FEC controller disabled\n");
		return -1;
	}
	if (length <= 14) {
		PRINTD("[!] Length too short\n");
		return -1;
	}

	if (length > 1500) {
		PRINTD("[!] Length too long\n");
		return -1;
	}

	/* Get current transmit buffer descriptor */
	volatile struct fec_bd * rbd = &tx_bd[tx_bd_index];

	/* Set data length */
	rbd->length = length;

	/* copy frame to send into tx buffer */
	memcpy (tx_buff, frame, length);

	/* Set data pointer */
	rbd->data = (uint32_t) tx_buff;

	/* Set status */
	uint16_t status = rbd->status & FEC_BD_TX_WRAP;
	status |= FEC_BD_TX_LAST | FEC_BD_TX_READY | FEC_BD_TX_TC;
	rbd->status = status;

	/* Tx active */
	fec->tdar = FEC_TDAR_TX_ACTIVE;

	/* Wait until frame is sent */
	int timeout = 50;
	while ((rbd->status & FEC_BD_TX_READY) && (timeout-- >= 0))
		usleep (100);

	if (timeout < 0) {
		PRINTD("[!] fec_send: timeout\n");
		return -1;
	}

	/* Set next transmit descriptor */
	tx_bd_index = (tx_bd_index + 1) % 2;
	PRINTD("[!] Packet sent");
	return 0;
}

