/*******************************************************************************
 * Chronometer Application - Source file
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdio.h>
#include <stdbool.h>

#include "chronometer.h"

#include "gpt.h"
#include "gpio.h"
#include "kernel.h"
#include "view_home.h"
#include "utils.h"
#include "kernel.h"
#include "consolemanager.h"
#include "model.h"

/*============================================================================*/
/* Local constants and types definitions                                      */
/*============================================================================*/
#define REFRESH_PERIOD 100 /**< Refresh period in ms */
#define BUTTON_START 3     /**< The GPIO pin for the button to start the chrono */
#define BUTTON_STOP 4      /**< The GPIO pin for the button to stop the chrono */
#define BUTTON_RESET 6     /**< The GPIO pin for the button to reset the chrono */

/**
 * States of the Chrono
 */
enum states {
	state_reset, ///< The chrono is reseted
	state_start, ///< The chrono is running
	state_stop   ///< The chrono is stop
};


/*============================================================================*/
/* Local variables                                                            */
/*============================================================================*/
/**
 * The time in ms
 */
static int ms = 0;
/**
 * The semaphore index for the display refresh
 */
static int sema = -1;
/**
 * The timer index for the interrupt counter
 */
static int timer = -1;

/**
 * The current state of the state machine
 */
static enum states state = state_reset;

/**
 * The display refresh thread status
 */
static bool run = false;

/*============================================================================*/
/* Local functions                                                            */
/*============================================================================*/

/**
 * Thread to refresh the display value
 * @param param UNUSED
 */
static void refresh_display(void* UNUSED(param)) {
	run = true;

	while(run) {

		view_home_set_time(chronometer_get_time());

		kernel_sema_wait(sema); //Wait ISR or stop
		kernel_reschedule();
	}
	kernel_thread_exit();
}
/**
 * The interrupt service routine fot the timer
 * @param UNUSED
 */
static void isr(void* UNUSED(param)) {
	ms += REFRESH_PERIOD;
	kernel_sema_signal(sema);
}
/**
 * The callback routine to start the chrono
 *
 * @param UNUSED
 */
static void start_chrono(void* UNUSED(param)) {
	//printf("start\n\r");
	if(state==state_reset) {
		gpt_timer_start_continuous(timer, REFRESH_PERIOD);
		state = state_start;
	} else if(state == state_stop) {
		gpt_timer_restart(timer);
		state = state_start;
	}
}
/**
 * The callback routine to stop the chrono
 * @param UNUSED
 */
static void stop_chrono(void* UNUSED(param)) {
	//printf("stop");
	if(state == state_start) {
		gpt_timer_stop(timer);
		state = state_stop;
	}
}
/**
 * The callback routine to reset the chrono
 * @param UNUSED
 */
static void reset_chrono(void* UNUSED(param)) {
	//printf("reset");
	if(state != state_reset) {
		gpt_timer_stop(timer);
		ms = 0;
		state = state_reset;
		kernel_sema_signal(sema);
	}
}

/*============================================================================*/
/* Global functions                                                           */
/*============================================================================*/

void chronometer_init() {
	consolemanager_println("Init: chronometer");

	//Buttons
	gpio_attach(GPIO_PORT_E, BUTTON_START, GPIO_IRQ_FALLING, start_chrono, 0);
	gpio_attach(GPIO_PORT_E, BUTTON_STOP, GPIO_IRQ_FALLING, stop_chrono, 0);
	gpio_attach(GPIO_PORT_E, BUTTON_RESET, GPIO_IRQ_FALLING, reset_chrono, 0);

	//Enable buttons
	gpio_enable(GPIO_PORT_E, BUTTON_START);
	gpio_enable(GPIO_PORT_E, BUTTON_STOP);
	gpio_enable(GPIO_PORT_E, BUTTON_RESET);

	//Timer
	timer = gpt_timer_create(GPT_FREQ_MS, isr, 0);
	sema = kernel_sema_create(0);
}

int chronometer_thread_start() {
	//Precondition
	if(run) return -1;

	kernel_thread_create(refresh_display, 0);

	model_start_app(app_chronometre);
	return 0;
}

void chronometer_thread_stop() {
	consolemanager_println("Stop thread: chronometer");

	run = false; //Stop the thread normally
	kernel_sema_signal_all(sema, 0); //Deblock the thread

	model_stop_app(app_chronometre);
}

struct utils_time chronometer_get_time() {
	//int hun = (ms / 10) % 1000;  //hundredth of a second
	struct utils_time time;

	time.s = (ms / 1000) % 60;
	time.m = (ms / (1000*60)) % 60;
	time.h = (ms / (1000*60*60));

	return time;
}

void chronometer_start() {
	//Precondition
	if (!run) return;
	start_chrono(0);
}
void chronometer_stop() {
	//Precondition
	if (!run) return;
	stop_chrono(0);
}
void chronometer_reset() {
	//Precondition
	if (!run) return;
	reset_chrono(0);
}
