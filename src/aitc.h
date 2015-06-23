/*******************************************************************************
 * ARM Interrupt Handling
 *
 * Module to deal with the ARM interrupt controller (AITC) of
 * 		the i.MX27.
 *
 * This module provides services to attach/detach an interrupt
 * service routine to each individual interrupt source of the
 * i.MX27. Only the default priority scheme of the i.MX27 is
 * currently implemented and all the sources will generate an IRQ.
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/
#pragma once
#ifndef AITC_H
#define AITC_H


#include <stdbool.h>


/**
 * List of available interrupt sources attached to AITC.
 * This list provides the vector of each interrupt source according
 * to the i.MX27 Reference Manual at chapter 10.
 *
 * A spurious interrupt vector has been added to enable application
 * software to treat unknown interrupts.
 */
enum aitc_vectors {
	AITC_SPURIOUS,	/**< Spurious interrupt */
	AITC_I2C2,	/**< I2C Bus Controller (I2C2) */
	AITC_GPT6,	/**< General Purpose Timer (GPT6) */
	AITC_GPT5,	/**< General Purpose Timer (GPT5) */
	AITC_GPT4,	/**< General Purpose Timer (GPT4) */
	AITC_RTIC,	/**< Real Time Integrity Checker (RTIC) */
	AITC_CSPI3,	/**< Configurable SPI 3 */
	AITC_MSHC,	/**< Memory Stick Host Controller (MSHC) */
	AITC_GPIO,	/**< General Purpose Input/Output (GPIO) */
	AITC_SDHC3,	/**< Secure Digital Host Controller (SDHC3) */
	AITC_SDHC2, 	/**< Secure Digital Host Controller (SDHC2) */
	AITC_SDHC1, 	/**< Secure Digital Host Controller (SDHC1) */
	AITC_I2C1, 	/**< I2C Bus Controller (I2C1) */
	AITC_SSI2,	/**< Synchronous Serial Interface (SSI2) */
	AITC_SSI1, 	/**< Synchronous Serial Interface (SSI1) */
	AITC_CSPI2, 	/**< Configurable SPI 2 */
	AITC_CSPI1, 	/**< Configurable SPI 1 */
	AITC_UART4, 	/**< UART 4 */
	AITC_UART3,	/**< UART 3 */
	AITC_UART2, 	/**< UART 2 */
	AITC_UART1, 	/**< UART 1 */
	AITC_KPP,	/**< Key Pad Port (KPP) */
	AITC_RTC,	/**< Real-Time Clock (RTC) */
	AITC_PWM,	/**< Pulse Width Modulator (PWM) */
	AITC_GPT3,	/**< General Purpose Timer (GPT3) */
	AITC_GPT2,	/**< General Purpose Timer (GPT2) */
	AITC_GPT1,	/**< General Purpose Timer (GPT1) */
	AITC_WDOG, 	/**< Watchdog (WDOG) */
	AITC_PCMCIA,	/**< PCMCIA/CF Host Controller (PCMCIA) */
	AITC_NFC,	/**< NAND Flash Controller (NFC) */
	AITC_ATA,	/**< Advanced Technology Attachment (ATA) Hard Disk */
	AITC_CSI,	/**< CMOS Sensor Interface (CSI) */
	AITC_DMACH0, 	/**< DMA Channel 0 */
	AITC_DMACH1,  	/**< DMA Channel 1 */
	AITC_DMACH2,  	/**< DMA Channel 2 */
	AITC_DMACH3,	/**< DMA Channel 3 */
	AITC_DMACH4,  	/**< DMA Channel 4 */
	AITC_DMACH5,  	/**< DMA Channel 5 */
	AITC_DMACH6,  	/**< DMA Channel 6 */
	AITC_DMACH7,	/**< DMA Channel 7 */
	AITC_DMACH8,  	/**< DMA Channel 8 */
	AITC_DMACH9,  	/**< DMA Channel 9 */
	AITC_DMACH10, 	/**< DMA Channel 10 */
	AITC_DMACH11,	/**< DMA Channel 11 */
	AITC_DMACH12, 	/**< DMA Channel 12 */
	AITC_DMACH13, 	/**< DMA Channel 13 */
	AITC_DMACH14, 	/**< DMA Channel 14 */
	AITC_DMACH15,	/**< DMA Channel 15 */
	AITC_UART6,	/**< UART 6 */
	AITC_UART5,	/**< UART 5 */
	AITC_FEC,	/**< Fast Ethernet Controller */
	AITC_EMMAPRP,	/**< eMMA Pre Processor */
	AITC_EMMAPP,	/**< eMMA Post Processor */
	AITC_H264,	/**< H264 */
	AITC_USBHS2, 	/**< USB HOST 1 */
	AITC_USBHS1,	/**< USB HOST 2 */
	AITC_USBOTG, 	/**< USB OTG */
	AITC_SMN,	/**< SCC SMN */
	AITC_SCM,	/**< SCC SCM */
	AITC_SAHARA, 	/**< Symmetric/Asymmetric Hashing and Random Accelerator */
	AITC_SLCDC, 	/**< Smart LCD Controller (SLCDC) */
	AITC_LCDC,	/**< LCD Controller (LCDC) */
	AITC_IIM, 	/**< IC Identify Module (IIM) */
	AITC_DPTC,	/**< Dynamic Process Temperature Compensate (DPTC) */
	AITC_NB_OF_VECTORS /**< number of vectors contained in the enumeration */
};

/**
 * Interrupt request types
 */
enum aitc_int_types {AITC_IRQ,  /**< to generate an IRQ */
		     AITC_FIQ	/**< to generate a FIQ */
};


/**
 * Prototype of the interrupt service routine
 *
 * @param param free user specified parameter
 */
typedef void (*aitc_service_routine_t) (void* param);


/**
 * initialization method
 * should be called prior any other method of this module
 */
extern void aitc_init();


/**
 * method to attach an application specific interrupt service routine to 
 * the specified vector
 *
 * @param vector interrupt vector to which the application ISR should be attached
 * @param type interrupt type (normal interrupt, fast interrupt)
 * @param routine application specific interrupt service routine
 * @param param application specific parameter passed to the ISR during call
 *
 * @return execution status: 0 <=> success, -1 <=> error
 */
extern int aitc_attach (
	enum aitc_vectors vector, 
	enum aitc_int_types type,
	aitc_service_routine_t routine, 
	void* param);


/**
 * method to detach application specific interrupt service routine from the 
 * specified vector
 *
 * @param vector interrupt vector from which the application ISR should be detached
 */
extern void aitc_detach (enum aitc_vectors vector);


/**
 * method to force/simulate an interrupt request
 *
 * @param vector interrupt vector for which the interrupt should be forced/simulated
 * @param force true to force the interrupt request, false otherwise
 */
extern void aitc_force (enum aitc_vectors vector, bool force);


#endif
