/*******************************************************************************
 * Thermometer Application - Header file
 *
 * This is the application for the Thermometer.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
 
#include "thermometer.h"

#include "utils.h"
#include "consolemanager.h"
#include "kernel.h"
#include "i2c.h"
#include "view_home.h"
#include "model.h"
#include "display.h"

/*============================================================================*/
/* Local constants and types definitions                                      */
/*============================================================================*/

/**
 * The LM75 bit fields.
 */
#define LM75_ID 0x48
#define LM75_TEMP_REG 0x00
#define LM75_CONFIG_REG 0x01
#define LM75_THYS_REG  0x02
#define LM75_TOS_REG 0x03

#define TEMP_POLL_INTERVAL 500 /**< The interval between two poll */

/*============================================================================*/
/* Local variables                                                            */
/*============================================================================*/

/**
 * The running status of the polling thread.
 */
static bool run = false;
/**
 * The running status of the refresh display thread.
 */
static bool display_run = false;

/**
 * The last temperature.
 */
struct utils_temperature last_temp = { 0, 0};

/**
 * The current temperature.
 */
struct utils_temperature temp;

/**
 * The semaphore for refreshing the display.
 */
int display_sem = -1;

/*============================================================================*/
/* Local functions                                                            */
/*============================================================================*/

/**
 * Get the current temperature.
 * @param temp the current temperature
 * @return -1 on error, 0 on success
 */
static int get_temp(struct utils_temperature* temp) {

	uint8_t buf[2] = {0,0};

	int status = i2c_read (LM75_ID, LM75_TEMP_REG, buf, 2);

	if (status != 0) return -1;

	temp->bdv = (int8_t)buf[0];
	temp->adv = buf[1] >= 128 ? 5 : 0;

	return 0;
}

/**
 * The refresh display thread.
 *
 * @param UNUSED
 */
static void display_thread(void* UNUSED(param)) {
	display_sem = kernel_sema_create(0);
	display_run = true;
	while(display_run) {
		view_home_set_temperature(temp);
		kernel_sema_wait(display_sem);
	}
	kernel_sema_destroy(display_sem);
	kernel_thread_exit();
}

/**
 * The polling thread
 *
 * @param UNUSED
 */
static void poll_thread(void* UNUSED(param)) {

	run = true;

	while(run) {
		if(get_temp(&temp) == 0){
			// Update only if temperature has changed
			if(last_temp.adv != temp.adv || last_temp.bdv != temp.bdv) {

				display_set(temp.bdv);

				kernel_sema_signal(display_sem);

				last_temp.adv = temp.adv;
				last_temp.bdv = temp.bdv;
			}
		}
		kernel_thread_sleep(TEMP_POLL_INTERVAL);
	}
	kernel_thread_exit();
}

/*============================================================================*/
/* Global functions                                                           */
/*============================================================================*/

void thermometer_init() {
	consolemanager_println("Init: thermometer");
}

void thermometer_display_thread_start() {
	if(display_run) return;

	kernel_thread_create(display_thread, 0);
}

void thermometer_display_thread_stop() {
	display_run = false;
	kernel_sema_signal(display_sem);
}

void thermometer_thread_start() {
	//Precondition
	if(run) return;

	display_enable();
	kernel_thread_create(poll_thread, 0);
	model_start_app(app_thermometre);
}

void thermometer_thread_stop() {
	run = false;

	display_disable();
	model_stop_app(app_thermometre);
}
