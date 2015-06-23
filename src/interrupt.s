/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract: 	ARM Interrupt Handling - Low Level Interface
 * Author: 	Daniel Gachet / EIA-FR / Telecoms
 * Date: 	11.03.2014
 */

/*-- exports --------------------------------------------------------------- */
		.global	interrupt_init, 
		.global interrupt_attach, interrupt_detach
		.global	interrupt_enable, interrupt_disable

/*
 * ARM9/i.MX27 interrupt vector table
 *
 * struct interrupt_arm_interrupt_vector_table
 * {
 * 	interrut_isr_t undef;   // 0xffff'fef0 
 * 	interrut_isr_t swi;     // 0xffff'fef4 
 * 	interrut_isr_t inst;    // 0xffff'fef8 
 * 	interrut_isr_t data;    // 0xffff'fefc 
 * 	interrut_isr_t irq;     // 0xffff'ff00 
 * 	interrut_isr_t fiq;     // 0xffff'fff4 
 * };
 */

/*-- local symbols --------------------------------------------------------- */
INT_UNDEF    = 0	/* undefined instruction */
INT_SWI      = 1 	/* software interrupt */
INT_PREFETCH = 2 	/* prefetch abort (instruction prefetch) */
INT_DATA     = 3 	/* data abort (data access) */
INT_IRQ      = 4	/* hardware interrupt request */
INT_FIQ      = 5	/* hardware fast interrupt request */

INT_VECTORS  = 6	/* number of interrupt vectors */

INT_STACK_SIZE = 0x4000	/* interrupt stack size */

INT_VRAM_BASE = 0xfffffef0 /* base addres of the VRAM vector table */


/*-- local variables ------------------------------------------------------- */
		.bss
.align 4
isr_routines:	.space 4*INT_VECTORS
isr_params:	.space 4*INT_VECTORS
.align 8
irq_stack:	.space INT_STACK_SIZE /* irq stack */
irq_stack_top:
fiq_stack:	.space INT_STACK_SIZE /* fiq stack */
fiq_stack_top:
undef_stack:	.space INT_STACK_SIZE /* undef stack */
undef_stack_top:
abort_stack:	.space INT_STACK_SIZE /* abort stack */
abort_stack_top:


/*-- public methods -------------------------------------------------------- */

		.text

interrupt_init:
	nop
	/* initialize the interrupt stacks*/
	mrs	r0, cpsr		/* save mode */

	msr     cpsr_c, #0xd1 		/* switch to fiq mode */
	ldr	sp, =fiq_stack_top
	msr     cpsr_c, #0xd2 		/* switch to irq mode */
	ldr	sp, =irq_stack_top
	msr     cpsr_c, #0xd7		/* switch to abort mode */
	ldr	sp, =abort_stack_top
	msr     cpsr_c, #0xdb 		/* switch to undef mode */
	ldr	sp, =undef_stack_top

	msr     cpsr_c, r0 		/* restore mode */

	/* attach lowlevel interrupt service routines to interrupt vector table */
	ldr	r1, =INT_VRAM_BASE		
	ldr	r0, =int_undef_sr
	str	r0, [r1, #4*INT_UNDEF]
	ldr	r0, =int_swi_sr
	str	r0, [r1, #4*INT_SWI]
	ldr	r0, =int_prefetch_sr
	str	r0, [r1, #4*INT_PREFETCH]
	ldr	r0, =int_data_sr
	str	r0, [r1, #4*INT_DATA]
	ldr	r0, =int_irq_sr
	str	r0, [r1, #4*INT_IRQ]
	ldr	r0, =int_fiq_sr
	str	r0, [r1, #4*INT_FIQ]

	bx	lr


interrupt_enable:
	nop
	mrs	r0, cpsr
	bic	r0, #0xc0
	msr	cpsr_c, r0
	bx	lr

interrupt_disable:
	nop
	mrs	r0, cpsr
	orr	r0, #0xc0
	msr	cpsr_c, r0
	bx	lr

interrupt_attach:
	nop
	ldr	r3, =isr_routines
	ldr	r3, [r3, r0, lsl #2]
	cmp	r3, #0
	movne	r0, #-1
	bxne	lr

	ldr	r3, =isr_routines
	str	r1, [r3, r0, lsl #2]
	ldr	r3, =isr_params
	str	r2, [r3, r0, lsl #2]
	mov	r0, #0
	bx	lr

interrupt_detach:
	nop
	mov	r1, #0
	ldr	r2, =isr_routines
	str	r1, [r2, r0, lsl #2]
	bx	lr


/*-- local methods  -------------------------------------------------------- */

/**
 * macro implementing low level isr entry operations
 */
.macro lowlevel_isr offset, vector
	nop	
	sub	lr, #\offset		/*adjust return address*/
	stmfd   sp!, {r0-r12,lr}	/*save context and return address*/
	mov	r1, #\vector		/*indicate vector number*/
	b	int_do_irq
.endm

/**
 * The methods have to be attached to the interrupt vectors 
 */
int_undef_sr: 	 lowlevel_isr 0, INT_UNDEF
int_swi_sr:	 lowlevel_isr 0, INT_SWI
int_prefetch_sr: lowlevel_isr 4, INT_PREFETCH
int_data_sr:	 lowlevel_isr 4, INT_DATA
int_irq_sr:	 lowlevel_isr 4, INT_IRQ
int_fiq_sr:	 lowlevel_isr 4, INT_FIQ

/**
 * Low level interrupt handler 
 */
int_do_irq:
	nop
	ldr	r2, =isr_params		/*load address of the C-param*/
	ldr	r2, [r2, r1, lsl #2]
	ldr	r3, =isr_routines	/*load address of the C-ISR*/
	ldr	r3, [r3, r1, lsl #2]
	cmp	r3, #0			/*test if an isr has been attached*/
	beq	int_do_irq	
	mov	r0, lr
	blx	r3			/*process the request (call C-ISR)*/

	ldmfd   sp!,{r0-r12,pc}^	/*restore the context, return and*/
					/* restore cpsr*/
