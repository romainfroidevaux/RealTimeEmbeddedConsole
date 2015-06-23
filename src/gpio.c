/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract: i.MX27 GPIO
 * Author: 	Daniel Gachet / EIA-FR / Telecoms
 * Date: 	29.01.2014
 */

#include <stdbool.h>

#include "gpio.h"
#include "aitc.h"
#include <stdio.h>

/* -- Internal types and constant definition -------------------------------- */

/**
 * GPIO Controller Register Definition (port A to F)
 */
struct gpio_ctrl {
	uint32_t ddir;
	uint32_t ocr[2];
	uint32_t iconfa[2];
	uint32_t iconfb[2];
	uint32_t dr;
	uint32_t gius;
	uint32_t ssr;
	uint32_t icr[2];
	uint32_t imr;
	uint32_t isr;
	uint32_t gpr;
	uint32_t swr;
	uint32_t puen;
};
static volatile struct gpio_ctrl* gpio [] = {
		(struct gpio_ctrl*)0x10015000, (struct gpio_ctrl*)0x10015100,
		(struct gpio_ctrl*)0x10015200, (struct gpio_ctrl*)0x10015300,
		(struct gpio_ctrl*)0x10015400, (struct gpio_ctrl*)0x10015500
};
static volatile uint32_t* GPIO_pmask = (uint32_t*)0x10015600;


/**
 * GPIO ISR Handler Structure Definition
 */
struct gpio_isr_handlers {
	gpio_isr_t routine;	/* application specific interrupt routine */
	void* param;		/* application specific parameter */
	enum gpio_modes mode;	/* pin operation mode */
};
static struct gpio_isr_handlers handlers[GPIO_NB_OF_PORTS][32];


/* -- Internal methods definition ------------------------------------------- */

static void gpio_isr(void* param __attribute__ ((unused)))
{
	for (enum gpio_ports port=0; port < GPIO_NB_OF_PORTS; port++) {
		volatile struct gpio_ctrl* gpioc = gpio[port];
		uint32_t isr = gpioc->isr & gpioc->imr;	/* compute interrupt sources */
		if (isr != 0) {
			gpioc->isr = isr;  /* acknowledged all pending sources */

			for (int pin=0; pin<32; pin++) {
				if (((isr & (1<<pin)) != 0) 
				& (handlers[port][pin].routine != 0)) 
				{
					handlers[port][pin].routine 
					  (handlers[port][pin].param);
				}
			}
		}
	}
}

/* -- Public methods definition --------------------------------------------- */

void gpio_init()
{
	printf("Init: gpio\n\r");

	aitc_attach(AITC_GPIO, AITC_IRQ, gpio_isr, 0);
	*GPIO_pmask = 0x3f;
}

/* ------------------------------------------------------------------------- */

int gpio_configure (enum gpio_ports port, uint32_t bitmask, enum gpio_modes mode)
{
	int status = -1;

	if (port < GPIO_NB_OF_PORTS) {
		volatile struct gpio_ctrl* gpioc = gpio[port];
		switch (mode) {
		case GPIO_OUTPUT:
			gpioc->dr &= ~bitmask;
			gpioc->ddir |= bitmask;
			for (int i=0;  i<32; i++) {
				if ((bitmask & 1<<i) != 0) {
					gpioc->ocr[i/16] |= 0x3<<((i%16)<<1);
				}
			}
			gpioc->gius |= bitmask;
			status = 0;
			break;

		case GPIO_INPUT:
			gpioc->ddir &= ~bitmask;
			for (int i=0; i<32; i++) if ((bitmask & 1<<i) != 0) {
				gpioc->iconfa[i/16] &= ~(0x3<<((i%16)<<1));
				gpioc->iconfb[i/16] &= ~(0x3<<((i%16)<<1));
			}
			gpioc->gius |= bitmask;
			gpioc->imr &= ~bitmask;
			status = 0;
			break;
		}
	}

	return status;
}

/* ------------------------------------------------------------------------- */

uint32_t gpio_getbits (enum gpio_ports port)
{
	if (port < GPIO_NB_OF_PORTS) {
		return (gpio[port]->ssr & ~(gpio[port]->ddir))
		     | (gpio[port]->dr & gpio[port]->ddir);
	}
	return 0;
}

/* ------------------------------------------------------------------------- */

void gpio_setbits (enum gpio_ports port, uint32_t bitmask)
{
	if (port < GPIO_NB_OF_PORTS) {
		gpio[port]->dr |= bitmask;
	}
}

/* ------------------------------------------------------------------------- */

void gpio_resetbits (enum gpio_ports port, uint32_t bitmask)
{
	if (port < GPIO_NB_OF_PORTS) {
		gpio[port]->dr &= ~(bitmask);
	}
}

/* ------------------------------------------------------------------------- */

int gpio_attach (
	enum gpio_ports port, 
	uint32_t pin, 
	enum gpio_interrupt_modes mode, 
	gpio_isr_t routine, 
	void* param)
{
	int status = -1;

	if ((port < GPIO_NB_OF_PORTS) && (pin < 32)
	&& (handlers[port][pin].routine == 0)) 
	{
		handlers[port][pin].routine = routine;
		handlers[port][pin].param = param;
                handlers[port][pin].mode = mode;

		volatile struct gpio_ctrl* gpioc = gpio[port];
		gpioc->imr &= ~(1<<pin);
                gpioc->isr = 1<<pin;
		gpioc->ddir &= ~(1<<pin);

		gpioc->iconfa[pin/16] &= ~(0x3<<((pin%16)<<1));
		gpioc->iconfa[pin/16] |=  (0x1<<((pin%16)<<1));
		gpioc->iconfb[pin/16] &= ~(0x3<<((pin%16)<<1));
		gpioc->iconfb[pin/16] |=  (0x1<<((pin%16)<<1));

		gpioc->icr[pin/16] &= ~(0x3<<((pin%16)<<1));
		gpioc->icr[pin/16] |= (mode<<((pin%16)<<1));

		gpioc->gius |= (1<<pin);

		status = 0;
	}

	return status;
}

/* ------------------------------------------------------------------------- */

void gpio_detach (enum gpio_ports port, uint32_t pin)
{
	if ((port < GPIO_NB_OF_PORTS) && (pin < 32)) {
		volatile struct gpio_ctrl* gpioc = gpio[port];
		gpioc->imr &= ~(1<<pin);
		gpioc->ddir &= ~(1<<pin);

		gpioc->iconfa[pin/16] &= ~(0x3<<((pin%16)<<1));
		gpioc->iconfb[pin/16] &= ~(0x3<<((pin%16)<<1));

		handlers[port][pin].routine = 0;
		handlers[port][pin].param = 0;
	}
}

/* ------------------------------------------------------------------------- */

void gpio_enable(enum gpio_ports port, uint32_t pin)
{
	if ((port < GPIO_NB_OF_PORTS) && (pin < 32)) {
		gpio[port]->imr |= 1<<pin;
	}
}

/* ------------------------------------------------------------------------- */

void gpio_disable(enum gpio_ports port, uint32_t pin)
{
	if ((port < GPIO_NB_OF_PORTS) && (pin < 32)) {
		gpio[port]->imr &= ~(1<<pin);
		gpio[port]->isr = 1<<pin;
	}
}


