/*******************************************************************************
* General Purpose Timer Driver - Header file
 *
* This is driver that provide services for using the six GPT.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef GPT_H
#define GPT_H

#include <stdbool.h>

#define GPT_FREQ_MS 1000 /**< Frequency for a period of 1 ms */

/**
 * List of available GPT vector
 */
enum timers {
	GPT_GPT1,
	GPT_GPT2,
	GPT_GPT3,
	GPT_GPT4,
	GPT_GPT5,
	GPT_GPT6,
	GPT_NB_OF_TIMERS,
};

/**
 * Type for the callback routine when an interrupt occur.
 *
 * @param The GPT that has enabled the interrupt
 * @param param Any optional parameter
 */
typedef void (*gpt_routine_t) (void* param);

/**
 * Initialize the GPT resources.
 */
extern void gpt_init();

/**
 * Configure a GPT timer.
 *
 * A timer must be configured only one time and before to use it.
 *
 * @param frequency the frequency of the counter in Hz
 * @param routine the routine for the interrupt callback
 * @param param the optional param for the interrupt callback
 * @return the index of the timer, -1 on error
 */
extern int gpt_timer_create(
		unsigned frequency,
		gpt_routine_t routine,
		void* param);

/**
 * Deconfigure the GPT timer.
 *
 * @param timer The timer to deconfigure
 * @return 0 on success, a negative value on error
 */
extern int gpt_timer_destroy(enum timers timer);

/**
 * Start the timer for one interrupt.
 *
 * The interrupt occur when the number of period is reached.
 *
 * @param timer the timer to start
 * @param period the number of period before the interrupt
 * @return 0 on success, a negative value on error
 */
extern int gpt_timer_start_oneshot(enum timers timer, int period);

/**
 * Start the timer for continuous interrupt.
 *
 * The interrupts occur when each number of period is reached.
 *
 * @param timer the timer to start
 * @param period the number of period before each interrupt
 * @return 0 on success, a negative value on error
 */
extern int gpt_timer_start_continuous(enum timers timer, int period);

/**
 * Stop the timer.
 *
 * The timer is stop at the current state.
 *
 * @param timer the timer to stop
 * @return 0 on success, a negative value on error
 */
extern int gpt_timer_stop(enum timers timer);

/**
 * Restart the timer.
 *
 * The timer restart from the last counter value.
 *
 * @param timer the timer to restart
 * @return 0 on success, a negative value on error
 */
extern int gpt_timer_restart(enum timers timer);

#endif
