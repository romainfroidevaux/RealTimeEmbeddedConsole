/**
 * EIA-FR - Embedded Systems 1 laboratory
 *
 * Abstract: 	LCLD - Liquid Crystal Display Controller
 *
 * Author: 	Daniel Gachet
 * Date: 	12.12.2013
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <imx27_clock.h>

#include "lcdc.h"

#define LCD_BITMAP_SIZE			(LCD_WIDTH*LCD_HEIGHT*2)

/* Bit definitions and macros for LSSAR */
#define LCDC_SSAR_SSA(x)		((x) & ~0x3)

/* Bit definitions and macros for LSR */
#define LCDC_SR_BUS_SIZE_32		(1 << 28)
#define LCDC_SR_XMAX(x)			((((x) >> 4) & 0x3f) << 20)
#define LCDC_SR_YMAX(x)			(x & 0x3ff)

/* Bit definitions and macros for LVPWR */
#define LCDC_VPWR_VPW(x)		((x) & 0x3ff)

/* Bit definitions and macros for LCPR */
#define LCDC_CPR_CC(x)			(((x) & 0x3) << 30)
#define LCDC_CPR_CC_AND			(3 << 30)
#define LCDC_CPR_CC_XOR			(2 << 30)
#define LCDC_CPR_CC_OR			(1 << 30)
#define LCDC_CPR_CC_TRANSPARENT	        (0 << 30)
#define LCDC_CPR_OP			(1 << 28)
#define LCDC_CPR_CXP(x)			(((x) & 0x3ff) << 16)
#define LCDC_CPR_CYP(x)			((x) & 0x3ff)

/* Bit definitions and macros for LCWHBR */
#define LCDC_CWHBR_BK_EN		(1 << 31)
#define LCDC_CWHBR_CW(x)		(((x) & 0x1f) << 24)
#define LCDC_CWHBR_CH(x)		(((x) & 0x1f) << 16)
#define LCDC_CWHBR_BD(x)		((x) & 0xff)

/* Bit definitions and macros for LCCMR */
#define LCDC_CCMR_CUR_COL_R(x)		(((x) & 0x3f) << 12)
#define LCDC_CCMR_CUR_COL_G(x)		(((x) & 0x3f) << 6)
#define LCDC_CCMR_CUR_COL_B(x)		((x) & 0x3f)
#define LCDC_CCMR_CUR_COL_RGB(r, g, b) 	(LCDC_CCMR_CUR_COL_R(r) | \
					 LCDC_CCMR_CUR_COL_G(g) | \
					 LCDC_CCMR_CUR_COL_B(b))

/* Bit definitions and macros for LPCR */
#define LCDC_PCR_PANEL_TYPE(x)	        (((x) & 0x3) << 30)
#define LCDC_PCR_MODE_TFT		(3 << 30)
#define LCDC_PCR_MODE_CSTN		(1 << 30)
#define LCDC_PCR_MODE_MONOCHROME        (0 << 30)
#define LCDC_PCR_TFT			(1 << 31)
#define LCDC_PCR_COLOR			(1 << 30)
#define LCDC_PCR_PBSIZ(x)		(((x) & 0x3) << 28)
#define LCDC_PCR_PBSIZ_8		(3 << 28)
#define LCDC_PCR_PBSIZ_4		(2 << 28)
#define LCDC_PCR_PBSIZ_2		(1 << 28)
#define LCDC_PCR_PBSIZ_1		(0 << 28)
#define LCDC_PCR_BPIX(x)		(((x) & 0x7) << 25)
#define LCDC_PCR_BPIX_18bpp		(6 << 25)
#define LCDC_PCR_BPIX_16bpp		(5 << 25)
#define LCDC_PCR_BPIX_12bpp		(4 << 25)
#define LCDC_PCR_BPIX_8bpp		(3 << 25)
#define LCDC_PCR_BPIX_4bpp		(2 << 25)
#define LCDC_PCR_BPIX_2bpp		(1 << 25)
#define LCDC_PCR_BPIX_1bpp		(0 << 25)
#define LCDC_PCR_PIXPOL			(1 << 24)
#define LCDC_PCR_FLMPOL			(1 << 23)
#define LCDC_PCR_LPPOL			(1 << 22)
#define LCDC_PCR_CLKPOL			(1 << 21)
#define LCDC_PCR_OEPOL			(1 << 20)
#define LCDC_PCR_SCLKIDLE		(1 << 19)
#define LCDC_PCR_ENDSEL			(1 << 18)
#define LCDC_PCR_SWAP_SEL		(1 << 17)
#define LCDC_PCR_REV_VS			(1 << 16)
#define LCDC_PCR_ACDSEL			(1 << 15)
#define LCDC_PCR_ACD(x)			(((x) & 0x7f) << 8)
#define LCDC_PCR_SCLKSEL		(1 << 7)
#define LCDC_PCR_SHARP			(1 << 6)
#define LCDC_PCR_PCD(x)			((x) & 0x3f)

/* Bit definitions and macros for LHCR */
#define LCDC_HCR_H_WIDTH(x)		(((x - 1) & 0x3f) << 26)
#define LCDC_HCR_H_WAIT_1(x)		(((x - 1) & 0xff) << 8)
#define LCDC_HCR_H_WAIT_2(x)		((x - 2) & 0xff)

/* Bit definitions and macros for LVCR */
#define LCDC_VCR_V_WIDTH(x)		(((x) & 0x3f) << 26)
#define LCDC_VCR_V_WAIT_1(x)		(((x) & 0xff) << 8)
#define LCDC_VCR_V_WAIT_2(x)		((x) & 0xff)

/* Bit definitions and macros for SCR */
#define LCDC_SCR_PS_R_DELAY(x)		(((x) & 0x3f) << 26)
#define LCDC_SCR_CLS_R_DELAY(x)		(((x) & 0xff) << 16)
#define LCDC_SCR_RTG_DELAY(x)		(((x) & 0xf) << 8)
#define LCDC_SCR_GRAY2(x)		(((x) & 0xf) << 4)
#define LCDC_SCR_GRAY1(x)		((x) & 0xf)

/* Bit definitions and macros for LPCCR */
#define LCDC_PCCR_CLS_HI_WID(x)	(((x) & 0x1FF) << 16)
#define LCDC_PCCR_LDMSK			(1 << 15)
#define LCDC_PCCR_SCR(x)		(((x) & 0x3) << 9)
#define LCDC_PCCR_SCR_LCDCLK		(2 << 9)
#define LCDC_PCCR_SCR_PIXCLK		(1 << 9)
#define LCDC_PCCR_SCR_LNPULSE		(0 << 9)
#define LCDC_PCCR_CC_EN			(1 << 8)
#define LCDC_PCCR_PW(x)			((x) & 0xff)

/* Bit definitions and macros for LDCR */
#define LCDC_DCR_BURST			(1 << 31)
#define LCDC_DCR_HM(x)			(((x) & 0x7f) << 16)
#define LCDC_DCR_TM(x)			((x) & 0x7f)

/* Bit definitions and macros for LRMCR */
#define LCDC_RMCR_SEL_REF		(1 << 0)

/* Bit definitions and macros for LICR */
#define LCDC_ICR_GW_INT_CON		(1 << 4)
#define LCDC_ICR_INTSYN			(1 << 2)
#define LCDC_ICR_INTCON			(1 << 0)

/* Bit definitions and macros for LIER */
#define LCDC_IER_GW_UDR			(1 << 7)
#define LCDC_IER_GW_ERR			(1 << 6)
#define LCDC_IER_GW_EOF			(1 << 5)
#define LCDC_IER_GW_BOF			(1 << 4)
#define LCDC_IER_UDR			(1 << 3)
#define LCDC_IER_ERR			(1 << 2)
#define LCDC_IER_EOF			(1 << 1)
#define LCDC_IER_BOF			(1 << 0)

/* Bit definitions and macros for LGWSAR */
#define LCDC_GWSAR_GWSA(x)		((x) & ~0x3)

/* Bit definitions and macros for LGWSR */
#define LCDC_GWSR_GWW(x)		((((x) >> 4) & 0x3F) << 20)
#define LCDC_GWSR_GWH(x)		((x) & 0x3FF)

/* Bit definitions and macros for LGWVPWR */
#define LCDC_GWVPWR_GWVPW(x)		((x) & 0x3ff)

/* Bit definitions and macros for LGWPOR */
#define LCDC_GWPOR_GWPO(x)		((x) & 0x1f)

/* Bit definitions and macros for LGWPR */
#define LCDC_GWPR_GWXP(x)		(((x) & 0x3ff) << 16)
#define LCDC_GWPR_GWYP(x)		((x) & 0x3ff)

/* Bit definitions and macros for LGWCR */
#define LCDC_GWCR_GWAV(x)		(((x) & 0xff) << 24)
#define LCDC_GWCR_GWCKE			(1 << 23)
#define LCDC_GWCR_GWE			(1 << 22)
#define LCDC_GWCR_GW_RVS		(1 << 21)
#define LCDC_GWCR_GWCKR(x)		(((x) & 0x3f) << 12)
#define LCDC_GWCR_GWCKG(x)		(((x) & 0x3f) << 6)
#define LCDC_GWCR_GWCKB(x)		((x) & 0x3f)

/* Bit definitions and macros for LGWDCR */
#define LCDC_GWDCR_GWBT			(1 << 31)
#define LCDC_GWDCR_GWHM(x)		(((x) & 0x7f) << 16)
#define LCDC_GWDCR_GWTM(x)		((x) & 0x7f)


/* LCDC registers */
struct lcdc_ctrl {
	uint32_t lssar;		/* Screen Start Address Register 		*/
	uint32_t lsr;		/* LCD Size Register 				*/
	uint32_t lvpwr;		/* Virtual Page Width Register 			*/
	uint32_t lcpr;		/* Cursor Position Register 			*/
	uint32_t lcwhb;		/* Cursor Width Height and Blink Register	*/
	uint32_t lccmr;		/* Color Cursor Mapping Register 		*/
	uint32_t lpcr;		/* Panel Configuration Register 		*/
	uint32_t lhcr;		/* Horizontal Configuration Register 		*/
	uint32_t lvcr;		/* Vertical Configuration Register 		*/
	uint32_t lpor;		/* Panning Offset Register 			*/
	uint32_t lscr;		/* Sharp Configuration Register 		*/
	uint32_t lpccr;		/* PWM Contrast Control Register 		*/
	uint32_t ldcr;		/* DMA Control Register 			*/
	uint32_t lrmcr;		/* Refresh Mode Control Register 		*/
	uint32_t licr;		/* Refresh Mode Control Register 		*/
	uint32_t lier;		/* Interrupt Enable Register 			*/
	uint32_t lisr;		/* Interrupt Status Register 			*/
	uint32_t res[3];	/* Reserved 					*/
	uint32_t lgwsar;	/* Graphic Window Start Address Register	*/
	uint32_t lgwsr;		/* Graphic Window Size Register 		*/
	uint32_t lgwvpwr;	/* Graphic Window Virtual Page Width Register	*/
	uint32_t lgwpor;	/* Graphic Window Panning Offset Register	*/
	uint32_t lgwpr;		/* Graphic Window Position Register 		*/
	uint32_t lgwcr;		/* Graphic Window Control Register 		*/
	uint32_t lgwdcr;	/* Graphic Window DMA Control Register 		*/
	uint32_t res1[5];	/* Reserved 					*/
	uint32_t lauscr;	/* Aus mode Controle Register 			*/
	uint32_t lausccr;	/* Aus mode Cursor Register 			*/
};
static volatile struct lcdc_ctrl* lcdc = (struct lcdc_ctrl*)0x10021000;


/* i.MX27 clocking registers */
#define PCCR0_LCDC_EN	(1 << 14)
#define PCCR1_HCLK_LCDC	(1 << 15)
static volatile uint32_t* clock_pcdr1 = (uint32_t*)0x1002701c;
static volatile uint32_t* clock_pccr0 = (uint32_t*)0x10027020;
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
		(struct gpio_ctrl*)0x10015000, (struct gpio_ctrl*)0x10015100, (struct gpio_ctrl*)0x10015200,
		(struct gpio_ctrl*)0x10015300, (struct gpio_ctrl*)0x10015400, (struct gpio_ctrl*)0x10015500
};


/* Bitmap allocation. Must be aligned to 4MB memory boundary. */
static uint16_t* lcdc_bitmap = (void*)0xb0000000; 
static uint16_t* lcdc_monitor = (void*)0xb0800000;

/* ------------------------------------------------------------------------- */

void lcdc_memory_space_swap() {
	if (lcdc_bitmap == (void*)0xb0000000)
		lcdc_bitmap = (void*)0xb0400000;
	else
		lcdc_bitmap = (void*)0xb0000000;

	/* Set screen start address */
	lcdc->lssar = (uint32_t)lcdc_bitmap;
}

/* ------------------------------------------------------------------------- */

static void lcdc_set_brightness (uint8_t level)
{
	/* Set LCDC PWM contrast control register */
	lcdc->lpccr = LCDC_PCCR_CLS_HI_WID(169) | LCDC_PCCR_SCR_PIXCLK |
					   LCDC_PCCR_CC_EN | level;
}

/* ------------------------------------------------------------------------- */

void lcdc_init ()
{
	/* Clear bits [31:5] of port A GIUS and GPR register */
	gpio[GPIO_PORT_A]->gius &= ~(0xffffffe0);
	gpio[GPIO_PORT_A]->gpr  &= ~(0xffffffe0);

	/* Configure port f pin 16 as output in set bit to 1 */
	gpio[GPIO_PORT_F]->ddir |= 1<<16; 
	gpio[GPIO_PORT_F]->ocr2 |= 3<<0;
	gpio[GPIO_PORT_F]->dr   |= 1<<16;
	gpio[GPIO_PORT_F]->puen &= ~(1<<16);

	/* Disable LCDC */
	lcdc_disable();

	/* Set PERDIV3 to 1 */
	*clock_pcdr1 &= ~(0xff << 16);
	*clock_pcdr1 |= 1 << 16;

	/* Clear the screen */
	lcdc_clear_screen();

	/* Set screen start address */
	lcdc->lssar = (uint32_t)lcdc_bitmap;

	/* Set screen size */
	lcdc->lsr = LCDC_SR_XMAX(LCD_WIDTH) | LCDC_SR_YMAX(LCD_HEIGHT);

	/* Set virtual page width */
	lcdc->lvpwr = LCDC_VPWR_VPW(LCD_WIDTH/2);

	/* Disable cursor */
	lcdc->lcwhb = 0;
	lcdc->lcpr = 0;
	lcdc->lccmr = 0;

	/* Calculate pixel clock divider */
	uint32_t pcd = (imx27_clock_get_perclk3() / LCD_FREQ) * 3 - 1;

	/* Set panel configuration register */
	lcdc->lpcr = LCDC_PCR_TFT | LCDC_PCR_COLOR | LCDC_PCR_PBSIZ_8 | 
		     LCDC_PCR_BPIX_16bpp | LCDC_PCR_FLMPOL | LCDC_PCR_LPPOL | 
		     LCDC_PCR_CLKPOL | LCDC_PCR_SCLKIDLE | LCDC_PCR_SCLKSEL | 
		     LCDC_PCR_PCD(pcd);

	/* Set horizontal configuration */
	lcdc->lhcr = LCDC_HCR_H_WIDTH(64) | LCDC_HCR_H_WAIT_1(96) | 
			LCDC_HCR_H_WAIT_2(96);

	/* Set vertical configuration */
	lcdc->lvcr = LCDC_VCR_V_WIDTH(16) | LCDC_VCR_V_WAIT_1(15) | 
			LCDC_VCR_V_WAIT_2(14);

	/* Set panning offset to */
	lcdc->lpor = 0;

	/* Set DMA control to 0 */
	lcdc->lscr = 0;

	/* Set contrast to 0 */
	lcdc->lpccr = 0;

	/* Set DMA control register */
	lcdc->ldcr = LCDC_DCR_TM(0x60) | LCDC_DCR_HM(0x04);

	/* Disable self-refresh */
	lcdc->lrmcr = 0;

	/* Disable interrupts */
	lcdc->licr = 0;
	lcdc->lier = 0;
	lcdc->lisr = 0;

	/* Disable graphic window, not yet defined  */
	lcdc->lgwsar = 0;  
	lcdc->lgwsr = 0;  
	lcdc->lgwvpwr = 0;
	lcdc->lgwpor = 0;
	lcdc->lgwpr = 0;
	lcdc->lgwdcr = 0;
	lcdc->lgwcr = 0;

	/* set aus mode registers to zero */
	lcdc->lauscr = 0;
	lcdc->lausccr = 0;
}

/* ------------------------------------------------------------------------- */

void lcdc_enable ()
{
	/* Enable clocks */
	*clock_pccr0 |= PCCR0_LCDC_EN;
	*clock_pccr1 |= PCCR1_HCLK_LCDC;

	/* Set brightness to 255 */
	lcdc_set_brightness(255);
}

/* ------------------------------------------------------------------------- */

void lcdc_disable ()
{
	/* Set brightness to 0 */
	lcdc_set_brightness(0);

	/* Disable clocks */
	*clock_pccr0 &= ~PCCR0_LCDC_EN;
	*clock_pccr1 &= ~PCCR1_HCLK_LCDC;
}

/* ------------------------------------------------------------------------- */

void lcdc_clear_screen ()
{
	/* Blank the screen */
	memset(lcdc_bitmap, 0xff, LCD_BITMAP_SIZE);
}

/* ------------------------------------------------------------------------- */

void lcdc_display_icon (
	uint16_t x, 
	uint16_t y, 
	uint16_t width, 
	uint16_t height, 
	uint16_t* icon)
{
	for (uint16_t i = 0; i < height; i++)
		memcpy(&lcdc_bitmap[(y+i)*LCD_WIDTH + x], 
			&icon[i*width], width*2);
}

/* ------------------------------------------------------------------------- */

uint16_t* lcdc_get_bitmap()
{
	return lcdc_bitmap;
}


/* ------------------------------------------------------------------------- */

uint16_t* lcdc_get_monitor()
{
	return lcdc_monitor;
}

/* ------------------------------------------------------------------------- */

void lcdc_define_monitor_position ( 
	uint16_t x, 
	uint16_t y, 
	uint16_t width, 
	uint16_t height)
{
	/* Set graphic window start address */
	lcdc->lgwsar = (uint32_t)lcdc_get_monitor();

	/* Set graphic window size */
	lcdc->lgwsr = LCDC_GWSR_GWW(width) | LCDC_GWSR_GWH(height);

	/* Set graphic window virtual page width */
	lcdc->lgwvpwr = LCDC_GWVPWR_GWVPW(width/2);

	/* Set graphic window panning offset  */
	lcdc->lgwpor = 0;

	/* Set graphic window position */
	lcdc->lgwpr = LCDC_GWPR_GWXP(x) | LCDC_GWPR_GWYP(y);

	/* Enable graphic window on screen and graphic window is totally opaque */
	lcdc->lgwcr = LCDC_GWCR_GWE | LCDC_GWCR_GWAV(0xff);

	/* Set graphic window DMA control register */
	lcdc->lgwdcr = LCDC_GWDCR_GWTM(0x60) | LCDC_GWDCR_GWHM(0x04);

	/* Disable and enable LCDC to activate configuration */
	lcdc_disable();
	lcdc_enable();
}

/* ------------------------------------------------------------------------- */

void lcdc_dump_regs()
{
	printf("[DEBUG] LCDC Registers\n");
	printf("[lssar]   %p - %lu\n", (void*)&lcdc->lssar, lcdc->lssar);
	printf("[lsr]     %p - %lx\n", (void*)&lcdc->lsr, lcdc->lsr);
	printf("[lvpwr]   %p - %lx\n", (void*)&lcdc->lvpwr, lcdc->lvpwr);
	printf("[lcpr]    %p - %lx\n", (void*)&lcdc->lcpr, lcdc->lcpr);
	printf("[lcwhb]   %p - %lx\n", (void*)&lcdc->lcwhb, lcdc->lcwhb);
	printf("[lccmr]   %p - %lx\n", (void*)&lcdc->lccmr, lcdc->lccmr);
	printf("[lpcr]    %p - %lx\n", (void*)&lcdc->lpcr, lcdc->lpcr);
	printf("[lhcr]    %p - %lx\n", (void*)&lcdc->lhcr, lcdc->lhcr);
	printf("[lvcr]    %p - %lx\n", (void*)&lcdc->lvcr, lcdc->lvcr);
	printf("[lpor]    %p - %lx\n", (void*)&lcdc->lpor, lcdc->lpor);
	printf("[lscr] 	  %p - %lx\n", (void*)&lcdc->lscr, lcdc->lscr);
	printf("[lpccr]   %p - %lx\n", (void*)&lcdc->lpccr, lcdc->lpccr);
	printf("[ldcr] 	  %p - %lx\n", (void*)&lcdc->ldcr, lcdc->ldcr);
	printf("[lrmcr]	  %p - %lx\n", (void*)&lcdc->lrmcr, lcdc->lrmcr);
	printf("[licr] 	  %p - %lx\n", (void*)&lcdc->licr, lcdc->licr);
	printf("[lier] 	  %p - %lx\n", (void*)&lcdc->lier, lcdc->lier);
	printf("[lisr] 	  %p - %lx\n", (void*)&lcdc->lisr, lcdc->lisr);
	printf("[lgwsar]  %p - %lx\n", (void*)&lcdc->lgwsar, lcdc->lgwsar);
	printf("[lgwsr]   %p - %lx\n", (void*)&lcdc->lgwsr, lcdc->lgwsr);
	printf("[lgwvpwr] %p - %lx\n", (void*)&lcdc->lgwvpwr, lcdc->lgwvpwr);
	printf("[lgwpor]  %p - %lx\n", (void*)&lcdc->lgwpor, lcdc->lgwpor);
	printf("[lgwpr]   %p - %lx\n", (void*)&lcdc->lgwpr, lcdc->lgwpr);
	printf("[lgwcr]   %p - %lx\n", (void*)&lcdc->lgwcr, lcdc->lgwcr);
	printf("[lgwdcr]  %p - %lx\n", (void*)&lcdc->lgwdcr, lcdc->lgwdcr);
	printf("[lauscr]  %p - %lx\n", (void*)&lcdc->lauscr, lcdc->lauscr);
	printf("[lausccr] %p - %lx\n", (void*)&lcdc->lausccr, lcdc->lausccr);
	printf("[/DEBUG]\n");
}
