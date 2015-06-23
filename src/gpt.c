/*******************************************************************************
 * General Purpose Timer Driver - Source file
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "gpt.h"

#include "aitc.h"
#include "kernel.h"
#include "consolemanager.h"


/*============================================================================*/
/* Local constants and types definitions                                      */
/*============================================================================*/

#define CLOCK_32K 32000 /**< 32 kHz */

/**
 * GPT Control Register Bit Fields
 */
#define TCTL_TEN          (1 <<  0) /**< Timer Enable */
#define TCTL_COMPEN       (1 <<  4) /**< Compare Interrupt Enable */
#define TCTL_CAPTEN       (1 <<  5) /**< Capture Interrupt Enable */
#define TCTL_CAP(x)       (((x) & 0x3) <<  6) /**< Capture Edge */
#define TCTL_FRR          (1 <<  8) /**< Free-Run/Restart */
#define TCTL_OM           (1 <<  9) /**< Output Mode */
#define TCTL_CC           (1 << 10) /**< Counter Clear */
#define TCTL_SWR          (1 << 15) /**< Software Reset */

/**
 * GPT Control Register Clock source Bit Field
 */
enum tctl_clocksource {
	none                = (0 << 1),///< Timer deactivated
	ipg_clk_perclk      = (1 << 1),///< Main clock (the divider must be > 2)
	ipg_clk_perclk_div4 = (2 << 1),///< Main clock divided by 4
	ipp_gpt_tin         = (3 << 1),///< External clock connected on the GPT TIN
	ipg_clk_32k         = (4 << 1) ///< 32 kHz clock (work in low power mode)
};

/**
 * GPT Prescaler Register Bit Fields
 */
#define TPRER_PRESCALER(x) (((x) & 0x3ff) <<  0) /**< Counter Clock Prescaler */
#define TPRER_MAX_VALUE 0x7ff /** Prescaler maximum value */

/**
 * GPT Status Register Bit Fields
 */
#define TSTAT_COMP (0x1 << 0)                /**< Compare Event */
#define TSTAT_CAPT (0x1 << 1)                /**< Capture Event */
#define TSTAT_ALL  (TSTAT_COMP | TSTAT_COMP) /**< All Events */



/**
 * GPT registers
 */
struct gpt_regs {
	uint32_t tctl; ///< Control Register
	uint32_t tprer; ///< Prescaler Register
	uint32_t tcmp; ///< Compare Register
	uint32_t tcr;  ///< Capture Register
	uint32_t tcn;  ///< Counter Register
	uint32_t tstat;  ///< Status Register
};

/**
 * GPT handler
 */
struct gpt_handler {
	gpt_routine_t routine;
	void* param;
	bool continuous; ///< True if the timer is periodical
};

typedef enum timers gpt_timers_t;

/*============================================================================*/
/* Local variables                                                            */
/*============================================================================*/
/**
 * Array of pointers to GPT registers
 */
static volatile struct gpt_regs* gpt[] = {
		(struct gpt_regs*) 0x10003000, ///< GPT1
		(struct gpt_regs*) 0x10004000, ///< GPT2
		(struct gpt_regs*) 0x10005000, ///< GPT3
		(struct gpt_regs*) 0x10009000, ///< GPT4
		(struct gpt_regs*) 0x1000a000, ///< GPT5
		(struct gpt_regs*) 0x1000f000  ///> GPT6
		};

/**
 * Look Up Table enum gpt_timer to enum aitc_vector
 */
static enum aitc_vectors timer_to_vector[] = {
		AITC_GPT1, ///< gpt_gpt1
		AITC_GPT2, ///< gpt_gpt2
		AITC_GPT3, ///< gpt_gpt3
		AITC_GPT4, ///< gpt_gpt4
		AITC_GPT5, ///< gpt_gpt5
		AITC_GPT6  ///< gpt_gpt6
};

/**
 * GPT handlers to attach a routine and a parameter
 */
static struct gpt_handler handlers[GPT_NB_OF_TIMERS];

/*============================================================================*/
/* Local functions                                                            */
/*============================================================================*/

/**
 * Interrupt service routine
 *
 * @pre timer is valid
 *
 * @param timer the timer to handle
 */
static void interrupt_service_routine(void* param) {
	enum timers timer = (enum timers) param;

	gpt[timer]->tstat = TSTAT_ALL; // Acknowledge

	if(!handlers[timer].continuous) {
		//Stop the timer
		gpt[timer]->tctl &= ~TCTL_TEN;
	}
	//Callback
	handlers[timer].routine(handlers[timer].param);
}

/*============================================================================*/
/* Global functions                                                           */
/*============================================================================*/

void gpt_init() {
	//Reset handlers routines and params to 0
	memset(handlers, '\0', sizeof(handlers));
}

int gpt_timer_create(
		unsigned frequency,
		gpt_routine_t routine,
		void* param) {


	//Preconditions
	if(routine == 0 || frequency == 0) return -1;

	//Calculate divider for clk32k
	unsigned divider = (CLOCK_32K / frequency) - 1;

	//Precondition
	if(divider > TPRER_MAX_VALUE) return -1;

	int timer = 0;

	while (timer < GPT_NB_OF_TIMERS && handlers[timer].routine != 0) timer++;

	//Free timer not found
	if (timer >= GPT_NB_OF_TIMERS){
		consolemanager_println("Error: no free timer");
		return -1;
	}


	//Software reset, disable timer and wait a minimum of 5 clock cycles
	gpt[timer]->tctl = TCTL_SWR;

	//Attach a routine and param
	handlers[timer].routine = routine;
	handlers[timer].param = param;

	//Attach interrupt service routine to AITC
	aitc_attach(timer_to_vector[timer], AITC_IRQ, interrupt_service_routine,
			(void*) timer);

	//Wait the end of software reset
	while((gpt[timer]->tctl & TCTL_SWR) != 0);

	//Choose fix 32kHz clock in restart mode with compare interrupt
	gpt[timer]->tctl = ipg_clk_32k | TCTL_COMPEN;
	gpt[timer]->tprer = divider;

	return timer;
}

int gpt_timer_destroy(enum timers timer) {
	//Precondition
	if((unsigned) timer >= GPT_NB_OF_TIMERS) return -1;

	handlers[timer].routine = 0;

	//Reset control register
	gpt[timer]->tctl = 0;
	return 0;
}

int gpt_timer_start_oneshot(enum timers timer, int period) {
	//Preconditions
	if((unsigned) timer >= GPT_NB_OF_TIMERS ||
			handlers[timer].routine == 0) return -1;

	handlers[timer].continuous = false;

	//Clear the counter
	gpt[timer]->tctl |= TCTL_CC;
	//Compare with the number of period
	gpt[timer]->tcmp = period;
	//Start the timer
	gpt[timer]->tctl |= TCTL_TEN;
	return 0;
}

int gpt_timer_start_continuous(enum timers timer, int period) {
	//Preconditions
	if((unsigned) timer >= GPT_NB_OF_TIMERS ||
			handlers[timer].routine == 0) return -1;

	handlers[timer].continuous = true;

	//Clear the counter
	gpt[timer]->tctl |= TCTL_CC;
	//Compare with the number of period
	gpt[timer]->tcmp = period;
	//Start the timer
	gpt[timer]->tctl |= TCTL_TEN;
	return 0;
}

int gpt_timer_stop(enum timers timer) {
	//Preconditions
	if((unsigned) timer >= GPT_NB_OF_TIMERS ||
			handlers[timer].routine == 0) return -1;

	//Stop the timer
	gpt[timer]->tctl &= ~TCTL_TEN;
	return 0;
}

int gpt_timer_restart(enum timers timer) {
	//Preconditions
	if((unsigned) timer >= GPT_NB_OF_TIMERS ||
			handlers[timer].routine == 0) return -1;

	//Start the timer without touching the compare and clear counter
	gpt[timer]->tctl |= TCTL_TEN;
	return 0;
}

