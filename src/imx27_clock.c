/**
 * EIA-FR - APF-27 Development Environment for LMI Labs
 *
 * Abstract: APF27 Clocking Module
 */

#include "imx27_clock.h"

/* Clock Source Control Register (CSCR) bit fields */
#define CSCR_MPEN		(1 << 0)	/* MPLL Enable */
#define CSCR_SPEN		(1 << 1)	/* Serial Peripheral PLL Enable */
#define CSCR_FPM_EN		(1 << 2)	/* Frequency Premultiplier */
#define CSCR_OSC26M_DIS		(1 << 3)	/* Oscillator Disable */
#define CSCR_OSC26M_DIV1P5	(1 << 4)	/* Oscillator 26M */
#define CSCR_AHB_DIV_SHF	8		/* AHB Div shift */
#define CSCR_AHB_DIV_MSK	(3<<8)		/* AHB Div mask */
#define CSCR_ARM_DIV_SHF	12		/* ARM Div shift */
#define CSCR_ARM_DIV_MSK	(3<<12)		/* ARM Div mask */
#define CSCR_ARM_SRC_MPLL	(1 << 15)	/* ARMSRC */
#define CSCR_MCU_SEL		(1 << 16)	/* MPLL Select */
#define CSCR_SP_SEL		(1 << 17)	/* SPLL Select */
#define CSCR_MPLL_RESTART	(1 << 18)	/* MPLL Restart */
#define CSCR_SPLL_RESTART	(1 << 19)	/* SPLL Restart */
#define CSCR_MSHC_SEL		(1 << 20)	/* MSHC CCLK Baud Source Select */
#define CSCR_H264_SEL		(1 << 21)	/* H264 Baud Source Select */
#define CSCR_SSI1_SEL		(1 << 22)	/* SSI1 Baud Source Select */
#define CSCR_SSI2_SEL		(1 << 23)	/* SSI2 Baud Source Select */
#define CSCR_SD_CNT_SHF		24		/* Shutdown control shift */
#define CSCR_SD_CNT_MSK		(7<<28)		/* Shutdown control mask */
#define CSCR_USB_DIV_SHF	28		/* USB Clock Divider shift */
#define CSCR_USB_DIV_MSK	(7<<28)		/* USR Clock Divider mask */
#define CSCR_UPDATE_DIS		(1 << 31)	/* Disable source selection and divider */

#define MPCTL1_BRMO		(1 << 6)
#define MPCTL1_LF		(1 << 15)


struct pll_clock_ctrl
{
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
static struct pll_clock_ctrl* pll_clock = (struct pll_clock_ctrl*)0x10027000;


static uint32_t imx27_clock_in_32k(void)
{
	return 1024 * 32768; /* 32768 or 32000 Hz crystal according to hardware population */
}

static uint32_t imx27_clock_in_26m(void)
{
	uint32_t imx27_clock_26m = 26000000;
	if (pll_clock->cscr & CSCR_OSC26M_DIV1P5) 
		imx27_clock_26m = 26000000 / 1.5;		/* divide by 1.5 */
	
	return imx27_clock_26m;
}

/*
 *  get the system pll imx27_clock in Hz
 *
 *                  mfi + mfn / (mfd +1)
 *  f = 2 * f_ref * --------------------
 *                        pd + 1
 */
static uint32_t imx27_clock_decode_pll (uint32_t pll, uint32_t f_ref)
{
	uint32_t mfi = (pll >> 10) & 0x00f;
	uint32_t mfn = (pll >>  0) & 0x3ff;
	uint32_t mfd = (pll >> 16) & 0x3ff;
	uint32_t pd  = (pll >> 26) & 0x00f;

	mfi = mfi <= 5 ? 5 : mfi;

	return (2ull * (unsigned long long)f_ref * (mfi*(mfd+1) + mfn)) / ((mfd+1)*(pd+1));
}


static uint32_t imx27_clock_get_mpllclk(void)
{
	uint32_t cscr = pll_clock->cscr;
	uint32_t fref;

	if (cscr & CSCR_MCU_SEL)
		fref = imx27_clock_in_26m();
	else
		fref = imx27_clock_in_32k();

	return imx27_clock_decode_pll(pll_clock->mpctl0, fref);
}



static uint32_t imx27_clock_decode_perclk (uint32_t div)
{
	return (imx27_clock_get_mpllclk() * 2) / (div * 3);
}


/* -------------------  */

uint32_t imx27_clock_get_ahbclk(void)
{
	return imx27_clock_decode_perclk (((pll_clock->cscr >>  8) & 0x03) + 1);
}

uint32_t imx27_clock_get_perclk1(void)
{
	return imx27_clock_decode_perclk(((pll_clock->pcdr1 >>  0) & 0x3f) + 1);
}

uint32_t imx27_clock_get_perclk2(void)
{
	return imx27_clock_decode_perclk(((pll_clock->pcdr1 >>  8) & 0x3f) + 1);
}

uint32_t imx27_clock_get_perclk3(void)
{
	return imx27_clock_decode_perclk(((pll_clock->pcdr1 >> 16) & 0x3f) + 1);
}

uint32_t imx27_clock_get_perclk4(void)
{
	return imx27_clock_decode_perclk(((pll_clock->pcdr1 >> 24) & 0x3f) + 1);
}

void imx27_clock_enable_line(enum imx27_clock_sources src)
{
	pll_clock->pccr0 |= 1<<src;

}

void imx27_clock_disable_line(enum imx27_clock_sources src)
{
	pll_clock->pccr0 &= ~(1<<src);
}



