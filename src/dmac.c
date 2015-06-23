/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract: ARM Direct Memory Access Controller
 *
 * Author:	Daniel Gachet / Alexandre Lefranc
 * Date: 	14.02.2014 
 */
#include <stdint.h>

#include "dmac.h"
#include "aitc.h"

/* -- Internal types and constant definition -------------------------------- */

/* DMA Control Register (DCR) bit fields */
#define DMAC_DCR_DMA 	(1<<2)		/* DMA access mode: 0=privileged, 1=user */
#define DMAC_DCR_DRST	(1<<1)		/* DMA soft reset: 0=no effect, 1=generate 3-cycle reset */
#define DMAC_DCR_DEN	(1<<0)		/* DMA enable: 0=disable, 1=enable */

/* DMA Burst Time-Out Register (DBTOCR) bit fields */
#define DMAC_DBTOCR_EN	(1<<15)		/* DMA burst time-out: 0=disable, 1=enables */

/**
 * DMA Controller Register Definition
 */
struct dmac_ctrl {
	uint32_t dcr; 		/* DMA control register */
	uint32_t disr;		/* DMA interrupt status register */
	uint32_t dimr;		/* DMA interrupt mask register */
	uint32_t dbtosr;	/* DMA burst time-out status register */
	uint32_t drtosr;	/* DMA request time-out status register */
	uint32_t dsesr;		/* DMA transfer error status register */
	uint32_t dbosr;		/* DMA buffer overflow status register */
	uint32_t dbtocr;	/* DMA burst time-out control register */
};
static volatile struct dmac_ctrl* dmac = (struct dmac_ctrl*)0x10001000;

#if 0
/**
 * DMA 2D Memory Register Definition
 */
struct dmac_size_reg_t
{
	uint32_t wsr;		/* W-size register */
	uint32_t xsr;		/* X-size register */
	uint32_t ysr;		/* Y-size register */
};
static struct dmac_size_reg_t* dmac_size_regs[2] = {
		(struct dmac_size_reg_t*)0x10001040,(struct dmac_size_reg_t*)0x1000104c
};
#endif

/* DMA Channel Control Register (CCR) bit fields */
#define DMAC_CCR_ACRPT		(1<<14)		/* auto clear rpt */
#define DMAC_CCR_DMOD_MSK 	(0x3<<12)	/* destination mode mask */
#define DMAC_CCR_DMOD_LINEAR	(0x0<<12)	/*  --> linear memory */
#define DMAC_CCR_DMOD_2D	(0x1<<12)	/*  --> 2D memory */
#define DMAC_CCR_DMOD_FIFO	(0x2<<12)	/*  --> FIFO memory */
#define DMAC_CCR_SMOD_MSK 	(0x3<<10)	/* source mode mask */
#define DMAC_CCR_SMOD_LINEAR	(0x0<<10)	/*  --> linear memory */
#define DMAC_CCR_SMOD_2D	(0x1<<10)	/*  --> 2D memory */
#define DMAC_CCR_SMOD_FIFO	(0x2<<10)	/*  --> FIFO memory */
#define DMAC_CCR_MDIR		(1<<9)		/* memory direction */
#define DMAC_CCR_MDIR_INCR	(0<<9)		/*  --> increment */
#define DMAC_CCR_MDIR_DECR	(1<<9)		/*  --> decrement */
#define DMAC_CCR_DSIZ_MSK	(0x3<<6)	/* destination size mask */
#define DMAC_CCR_DSIZ_32BIT	(0x0<<6)	/*   --> 32-bit port */
#define DMAC_CCR_DSIZ_8BIT	(0x1<<6)	/*   --> 8-bit port  */
#define DMAC_CCR_DSIZ_16BIT 	(0x2<<6)	/*   --> 16-bit port */
#define DMAC_CCR_SSIZ_MSK	(0x3<<4)	/* source size mask */
#define DMAC_CCR_SSIZ_32BIT	(0x0<<4)	/*   --> 32-bit port */
#define DMAC_CCR_SSIZ_8BIT	(0x1<<4)	/*   --> 8-bit port  */
#define DMAC_CCR_SSIZ_16BIT 	(0x2<<4)	/*   --> 16-bit port */
#define DMAC_CCR_REN		(1<<3)		/* request enable: enables/disables DMA request signal */
#define DMAC_CCR_RPT		(1<<2)		/* repeat: enables/disables data transfer repeat function */
#define DMAC_CCR_FRC		(1<<1)		/* force DMA cycle */
#define DMAC_CCR_CEN		(1<<0)		/* DMA channel enable */

/* DMA Channel Request Time-Out Register (RTOR) bit field */
#define DMAC_RTOR_EN		(1<<15)		/* Enable DMA request time-out */
#define DMAC_RTOR_CLK_32K	(1<<14)		/* Clock source: 0=HCLK, 1=32.768 kHz */
#define DMAC_RTOR_PSC		(1<<13)		/* Clock prescaler: 0=1, 1=256 */

/**
 * DMA Channel Register Definition
 */
struct dmac_channel {
	uint32_t sar;		/* source address register */
	uint32_t dar;		/* destination address register */
	uint32_t cntr;		/* count register */
	uint32_t ccr;		/* control register */
	uint32_t rssr;		/* request source select register */
	uint32_t blr;		/* burst length register */
	uint32_t rtor_bucr;	/* request time-out and bus utilization control register */
	uint32_t ccnr;		/* channel counter register */
};
static volatile struct dmac_channel* dmac_channels[] = {
		(struct dmac_channel*)0x10001080, (struct dmac_channel*)0x100010c0,
		(struct dmac_channel*)0x10001100, (struct dmac_channel*)0x10001140,
		(struct dmac_channel*)0x10001180, (struct dmac_channel*)0x100011c0,
		(struct dmac_channel*)0x10001200, (struct dmac_channel*)0x10001240,
		(struct dmac_channel*)0x10001280, (struct dmac_channel*)0x100012c0,
		(struct dmac_channel*)0x10001300, (struct dmac_channel*)0x10001340,
		(struct dmac_channel*)0x10001380, (struct dmac_channel*)0x100013c0,
		(struct dmac_channel*)0x10001400, (struct dmac_channel*)0x10001440
};


/**
 * DMA Channel ISR Handler Structure Definition
 */
struct dmac_isr_handlers {
	dmac_isr_t routine;  			/* application specific interrupt routine */
	void* handle;		 		/* application specific parameter */
	enum dmac_channels channel;		/* channel number */
	volatile struct dmac_channel* dma_channel;	/* pointer to channel registers */
};
static struct dmac_isr_handlers handlers[DMAC_NB_OF_CHANNELS];


/**
 * DMA Channel Number to AITC Interrupt Vector Translation Table
 */
static const enum aitc_vectors dmac2int[] = {
		AITC_DMACH0,  AITC_DMACH1,  AITC_DMACH2,  AITC_DMACH3,
		AITC_DMACH4,  AITC_DMACH5,  AITC_DMACH6,  AITC_DMACH7,
		AITC_DMACH8,  AITC_DMACH9,  AITC_DMACH10, AITC_DMACH11,
		AITC_DMACH12, AITC_DMACH13, AITC_DMACH14, AITC_DMACH15,
};


/**
 * PLL Clock Control Register Definition
 */
struct pll_clock_ctrl {
	uint32_t cscr;
	uint32_t mpctl0;
	uint32_t mpctl1;
	uint32_t spctl0;
	uint32_t spctl1;
	uint32_t osc26mctl;
	uint32_t pcdr0;
	uint32_t pcdr1;
	uint32_t pccr0;
	uint32_t pccr1;
	uint32_t reserved[(0x34-0x2c)/4];
	uint32_t wkgdctrl;
};
static volatile struct pll_clock_ctrl* pll_clock = (struct pll_clock_ctrl*)0x10027000;


/* -- Internal methods definition ------------------------------------------- */

/**
 * DMA Channel Interrupt Service Routine
 */
static void dmac_isr(void* handle)
{
	struct dmac_isr_handlers* oref = (struct dmac_isr_handlers*) handle;
	uint32_t msk = 1 << oref->channel;
	enum dmac_transfer_status status = DMAC_SUCCESS;

	dmac->disr = msk;			/* acknowledge interrupt */
	if ((dmac->dbtosr & msk) != 0) {	/* check for burst time-out */
		status = DMAC_BURST_TIMEOUT;
		dmac->dbtosr = msk;
	}
	if ((dmac->drtosr & msk) != 0) {	/* check request time-out */
		status = DMAC_REQUEST_TIMEOUT;
		dmac->drtosr = msk;
	}
	if ((dmac->dsesr & msk) != 0) {		/* check for transfer error */
		status = DMAC_TRANSFER_TIMEOUT;
		dmac->dsesr = msk;
	}
	if ((dmac->dbosr & msk) != 0) {		/* check for buffer overflow */
		status = DMAC_BUFFER_OVERFLOW;
		dmac->dbosr = msk;
	}

	/* call application specific interrupt service routine */
	oref->routine (oref->handle, status, oref->dma_channel->ccnr);
}


/* -- Public methods definition --------------------------------------------- */

void dmac_init()
{
	pll_clock->pccr0 |= 1<<28;	/* enable DMA clock --> should be moved in clocking module... */
	pll_clock->pccr1 |= 1<<20;

	dmac->dimr = -1;		/* mask all interrupt */
	dmac->disr = -1;		/* ackn all pending interrupt */
	dmac->dbtosr = -1;		/* ackn all burst time-out */
	dmac->drtosr = -1;		/* ackn all request time-out */
	dmac->dsesr = -1;		/* ackn all transfer error */
	dmac->dbosr = -1;		/* ackn all buffer overflow */
	dmac->dcr = DMAC_DCR_DEN;	/* enable DMA operation */
	dmac->dbtocr = DMAC_DBTOCR_EN + 512; /* enable 512 clock burst time-out */
}

/* ------------------------------------------------------------------------- */

int  dmac_attach(enum dmac_channels channel, dmac_isr_t routine, void* handle)
{
	int status = -1;
	if (handlers[channel].routine == 0) {
		handlers[channel].routine = routine;
		handlers[channel].handle = handle;
		handlers[channel].channel = channel;
		handlers[channel].dma_channel = dmac_channels[channel];
		aitc_attach (dmac2int[channel], AITC_IRQ, dmac_isr, &handlers[channel]);
		status = 0;
	}
	return status;
}

/* ------------------------------------------------------------------------- */

void dmac_detach(enum dmac_channels channel)
{
	aitc_detach (dmac2int[channel]);
	handlers[channel].routine = 0;
	handlers[channel].handle = 0;
}

/* ------------------------------------------------------------------------- */

int dmac_memmov (enum dmac_channels channel,
		 void* dst_addr,
                 const void* src_addr,
		 uint32_t size)
{
	if (channel >= DMAC_NB_OF_CHANNELS) return -1; 
	if (handlers[channel].routine == 0) return -1;

	volatile struct dmac_channel* dma = dmac_channels[channel];

	dmac->dimr &= ~(1<<channel);	/* enable channel interrupt */
	dmac->disr = 1<<channel;  	/* ackn pending interrupt */
	dmac->dbtosr = 1<<channel;	/* ackn burst time-out */
	dmac->drtosr = 1<<channel;	/* ackn request time-out */
	dmac->dsesr = 1<<channel; 	/* ackn transfer error */
	dmac->dbosr = 1<<channel; 	/* ackn buffer overflow */

	dma->ccr =  0; 			/* disable DMA */

	dma->sar = (uint32_t)src_addr;
	dma->dar = (uint32_t)dst_addr;
	dma->cntr = size;
	dma->blr = 0; 			/* 64 bytes burst length */
	dma->rtor_bucr = 0;		/* bus utilization 128 cycles before DMA start */

	dma->ccr = DMAC_CCR_DMOD_LINEAR | DMAC_CCR_DSIZ_32BIT
		 | DMAC_CCR_SMOD_LINEAR | DMAC_CCR_SSIZ_32BIT
		 | DMAC_CCR_MDIR_INCR
		 | DMAC_CCR_CEN;

	return 0;
}

