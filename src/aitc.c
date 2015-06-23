/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract:	ARM Interrupt Handling
 *
 * Author: 	Daniel Gachet
 * Date: 	29.01.2014
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "aitc.h"
#include "interrupt.h"

/**
 * AITC controller definition
 */
struct aitc_ctrl {
	uint32_t intcntl;
	uint32_t nimask;
	uint32_t intennum;
	uint32_t intdisnum;
	uint32_t intenable[2];
	uint32_t inttype[2];
	uint32_t nipriority[8];
	uint32_t nivecsr;
	uint32_t fivecsr;
	uint32_t intsrc[2];
	uint32_t intfrc[2];
	uint32_t nipnd[2];
	uint32_t fipnd[2];
};
static volatile struct aitc_ctrl* aitc = (struct aitc_ctrl*) 0x10040000;

/**
 * AITC vector table containing all the application specific
 * interrupt service routines with their parameter
 */
struct aitc_handler {
	aitc_service_routine_t routine;
	void* param;
}; 
static struct aitc_handler handlers[AITC_NB_OF_VECTORS];


/*-- local methods --------------------------------------------------------- */

/**
 * Normal/fast interrupt service routine. This method will look at the AITC to
 * get the highest pending interrupt vector and to call the related service
 * routine.
 */
void aitc_irq_isr(
	void* addr __attribute__((unused)), 
	enum interrupt_vectors int_vector, 
	void* param __attribute__((unused)))
{
	uint32_t vector = 0;
	// get IRQ interrupt vector from AITC and extract the vector
	if (int_vector == INT_IRQ) 
		vector = (aitc->nivecsr >> 16) & 0xff;
	else 
		vector = (aitc->fivecsr >>  0) & 0xff;

	if (handlers[vector].routine != 0) {
		handlers[vector].routine (handlers[vector].param);
	} else {
		// spurious interrupt
		aitc->intdisnum = vector;
		if (handlers[AITC_SPURIOUS].routine != 0)
			handlers[AITC_SPURIOUS].routine 
				(handlers[AITC_SPURIOUS].param);
	}
}


/*-- public methods -------------------------------------------------------- */

void aitc_init()
{
	printf("Init: aitc\n\r");

	memset (handlers, '\0', sizeof(handlers));
	interrupt_attach (INT_IRQ, aitc_irq_isr, 0);
	interrupt_attach (INT_FIQ, aitc_irq_isr, 0);

	aitc->intcntl = 0;
	aitc->nimask = -1;
	aitc->intenable[0] = 0;	/* disable all sources */
	aitc->intenable[1] = 0;
	aitc->inttype[0] = 0;	/* mark all sources as IRQ */
	aitc->inttype[1] = 0;
	aitc->intfrc[0] = 0;
	aitc->intfrc[1] = 0;
	for (int i=0; i<8; i++) {
		aitc->nipriority[i] = 0; /* mark all sources as lowest priority */
	}
}

/* ------------------------------------------------------------------------- */

int aitc_attach (
	enum aitc_vectors vector, 
	enum aitc_int_types type,
	aitc_service_routine_t routine, 
	void* param)
{
	int status = -1;

	if ((vector < AITC_NB_OF_VECTORS) && (handlers[vector].routine == 0)) {
		handlers[vector].routine = routine;
		handlers[vector].param = param;

		if (type == AITC_FIQ) {
			aitc->inttype[1-(vector/32)] |=  (1<<(vector%32));
		} else {
			aitc->inttype[1-(vector/32)] &= ~(1<<(vector%32));
		}
		aitc->intennum = vector;

		status = 0;
	}

	return status;
}

/* ------------------------------------------------------------------------- */

void aitc_detach (enum aitc_vectors vector)
{
	aitc->intdisnum = vector;

	if (vector < AITC_NB_OF_VECTORS) {
		handlers[vector].routine = 0;
		handlers[vector].param = 0;
	}
}

/* ------------------------------------------------------------------------- */

void aitc_force (enum aitc_vectors vector, bool force)
{
	if ((vector != AITC_SPURIOUS) && (vector < AITC_NB_OF_VECTORS)) {
		if (force) aitc->intfrc[1-(vector/32)] |=  (1<<(vector%32));
		else       aitc->intfrc[1-(vector/32)] &= ~(1<<(vector%32));
	}
}

