/*******************************************************************************
 * i.MX27 GPIO
 *
 * Module to deal with the general purpose I/O (GPIO) the i.MX27.
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/**
 * List of GPIO ports of the i.MX27
 */
enum gpio_ports {
	GPIO_PORT_A, 		/**< GPIO port A */
	GPIO_PORT_B, 		/**< GPIO port B */
	GPIO_PORT_C, 		/**< GPIO port C */
	GPIO_PORT_D, 		/**< GPIO port D */
	GPIO_PORT_E, 		/**< GPIO port E */
	GPIO_PORT_F, 		/**< GPIO port F */
	GPIO_NB_OF_PORTS	/**< number of GPIO ports */
	};


/**
 * GPIO port operation modes
 */
enum gpio_modes {
	GPIO_INPUT, 	/**< GPIO port will be used as input */
	GPIO_OUTPUT	/**< GPIO port will be used as output */
	};


/**
 * GPIO port interrupt modes
 */
enum gpio_interrupt_modes {
	GPIO_IRQ_RISING, 	/**< interrupt on rising edge */
	GPIO_IRQ_FALLING,	/**< interrupt on falling edge */
	GPIO_IRQ_HIGH, 		/**< interrupt on high level */
	GPIO_IRQ_LOW		/**< interrupt on low level */
};


/**
 * Prototype of the interrupt service routine
 *
 * @param param application specific parameter 
 */
typedef void (*gpio_isr_t) (void* param);


/**
 * method to initialize the GPIO resources
 * should be called prior any other methods of this module
 */
extern void gpio_init();


/**
 * method to configure I/O ports either as output or as interrupt source (falling edge).
 *
 * @param port port number to be configured
 * @param bitmask list of pins to be configured
 * @param mode operation mode of the pins to be configured
 * @return execution status (0=success, -1=error)
 */
extern int gpio_configure (enum gpio_ports port, uint32_t bitmask, enum gpio_modes mode);


/**
 * method to get the status of the specified pin
 *
 * @param port port number to be configured
 * @return bitmask 
 */
extern uint32_t gpio_getbits (enum gpio_ports port);


/**
 * method to set all specified pins to 1
 *
 * @param port port number to be configured
 * @param bitmask list of pins to be configured
 */
extern void gpio_setbits (enum gpio_ports port, uint32_t bitmask);


/**
 * method to reset all specified pins to 0
 *
 * @param port port number to be configured
 * @param bitmask list of pins to be configured
 */
extern void gpio_resetbits (enum gpio_ports port, uint32_t bitmask);


/**
 * method to attach an interrupt service routine a pin previously 
 * configured as interrupt source, the interrupt source is disabled
 * after attachment.
 *
 * @param port port number to which the ISR should be attached
 * @param pin pin number to which the ISR should be attached
 * @param mode interrupt mode
 * @param routine application specific interrupt service routine
 * @param param paramter passed as argument to the ISR when called
 * @return execution status (0=success, -1=error)
 */
extern int gpio_attach (
	enum gpio_ports port, 
	uint32_t pin, 
	enum gpio_interrupt_modes mode, 
	gpio_isr_t routine, 
	void* param);

/**
 * method to detach the interrupt service routine
 *
 * @param port port number from which the ISR should be detached
 * @param pin pin number from which the ISR should be detached
 */
extern void gpio_detach (enum gpio_ports port, uint32_t pin);

/**
 * method to enable interrupt on the specified pin
 *
 * @param port port number to be enabled
 * @param pin pin number to be enabled
 */
extern void gpio_enable(enum gpio_ports port, uint32_t pin);


/**
 * method to disable interrupt on the specified pin
 *
 * @param port port number to be disabled
 * @param pin pin number to be disabled
 */
extern void gpio_disable(enum gpio_ports port, uint32_t pin);


#endif
