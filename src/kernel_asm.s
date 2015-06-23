/**
 * EIA-FR - Embedded Systems 2
 *
 * Abstract: 	Introduction to Operating System implementation
 * Purpose:	Implementation of lowlevel services
 * Author: 	Daniel Gachet / EIA-FR / Telecoms
 * Date: 	28.03.2014
 */

/* Export public symbols */
	.global kernel_transfer

	.text
/* void kernel_transfer(struct tcb* former, uint32_t psr, struct tcb* new);
 * r0 = tcb address of the former thread (former)
 * r1 = psr of the former thread (psr)
 * r2 = tcb address of the new thread (new)
 */
kernel_transfer:
	nop
	stmia	r0,{r0-r12,sp,lr}
	str	r1,[r0, #15*4]

	ldr	r1,[r2, #15*4]
	msr	spsr_cxsf, r1
	ldmia	r2, {r0-r12,sp,pc}^

