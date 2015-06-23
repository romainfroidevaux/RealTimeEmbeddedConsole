#pragma once
#ifndef INTERRUPT_H
#define INTERRUPT_H
/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract: 	ARM Interrupt Handling - Low Level Interface
 *  
 * Purpose:	Module to deal with the low level ARM9/i.MX27 
 *		microprocessor interrupt logic
 *
 * Author: 	Daniel Gachet / EIA-FR / Telecoms
 * Date: 	29.01.2014
 */


/**
 * i.MX27 interrupt vectors enumération
 */
enum interrupt_vectors {
	INT_UNDEF,	/**< undefined instruction */
	INT_SWI, 	/**< software interrupt */
	INT_PREFETCH, 	/**< prefetch abort (instruction prefetch) */
	INT_DATA, 	/**< data abort (data access) */
	INT_IRQ, 	/**< hardware interrupt request */
	INT_FIQ};	/**< hardware fast interrupt request */

/**
 * Prototype of the interrupt service routines
 *
 * @param addr return address
 * @param vector i.MX27 interrupt vector
 * @param param parameter specified while attaching the interrupt 
 *              service routine 
 */
typedef void (*interrupt_isr_t) (void* addr, 
				 enum interrupt_vectors vector, 
				 void* param);


/**
 * Method to initialize low level resources of the ARM9 microprocessor
 * A 16KB of memory will be allocated for each interrupt vector
 */
extern void interrupt_init();


/**
 * Method to attach an interrupt servicde routine to the i.MX27 interrupt 
 * vector table
 *
 * @param vector i.MX27 interrupt vector
 * @param routine interrupt service routine to attach to the specified vector
 * @param param parameter to be passed as argument while calling the the s
 *              pecified ISR
 * @return execution status, 0 if sussess, -1 if an ISR is already attached
 */
extern int interrupt_attach (enum interrupt_vectors vector, 
			     interrupt_isr_t routine, 
			     void* param);


/**
 * Method to deattach an interrupt service routine from the i.MX27 interrupt
 * vector table
 *
 * @param vector i.MX27 interrupt vector
 */
extern void interrupt_detach (enum interrupt_vectors vector);


/**
 * Method to enable both normal and fast interrupt requests (IRQ/FIQ)
 */
extern void interrupt_enable();


/**
 * Method to disable both normal and fast interrupt requests (IRQ/FIQ)
 */
extern void interrupt_disable();


#endif
