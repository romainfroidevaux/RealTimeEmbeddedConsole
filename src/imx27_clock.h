#ifndef IMX27_CLOCK_H
#define IMX27_CLOCK_H
/**
 * EIA-FR - APF-27 Development Environment for LMI Labs
 *
 * Abstract: 	APF27 Clocking Module for newlib system calls
 *
 * Purpose:	This module provides several methods to deal with the
 * 		i.MX27 imx27_clocking module
 *
 */

#include <stdint.h>

/**
 * Definition of the imx27_clock sources which are used by i.MX27 internal
 * peripherals.
 */
enum imx27_clock_sources
{
	IMX27_IMX27_CLOCK_SSI2,
	IMX27_CLOCK_SSI1,
	IMX27_CLOCK_SLCDC,
	IMX27_CLOCK_SDHC3,
	IMX27_CLOCK_SDHC2,
	IMX27_CLOCK_SDHC1,
	IMX27_CLOCK_SCC,
	IMX27_CLOCK_SAHARA,
	IMX27_CLOCK_RTIC,
	IMX27_CLOCK_RTC,
	IMX27_CLOCK_reserved,
	IMX27_CLOCK_PWM,
	IMX27_CLOCK_OWIRE,
	IMX27_CLOCK_MSHC,
	IMX27_CLOCK_LCDC,
	IMX27_CLOCK_KPP,
	IMX27_CLOCK_IIM,
	IMX27_CLOCK_I2C2,
	IMX27_CLOCK_I2C1,
	IMX27_CLOCK_GPT6,
	IMX27_CLOCK_GPT5,
	IMX27_CLOCK_GPT4,
	IMX27_CLOCK_GPT3,
	IMX27_CLOCK_GPT2,
	IMX27_CLOCK_GPT1,
	IMX27_CLOCK_GPIO,
	IMX27_CLOCK_FEC,
	IMX27_CLOCK_EMMA,
	IMX27_CLOCK_DMA,
	IMX27_CLOCK_CSPI3,
	IMX27_CLOCK_CSPI2,
	IMX27_CLOCK_CSPI1
};

/* Methods to get the frequency of the specified imx27_clock line */
extern uint32_t imx27_clock_get_ahbclk(void);
extern uint32_t imx27_clock_get_perclk1(void);
extern uint32_t imx27_clock_get_perclk2(void);
extern uint32_t imx27_clock_get_perclk3(void);
extern uint32_t imx27_clock_get_perclk4(void);

/* Methods to enable/disable a imx27_clock source line */
extern void imx27_clock_enable_line (enum imx27_clock_sources src);
extern void imx27_clock_disable_line(enum imx27_clock_sources src);

#endif
