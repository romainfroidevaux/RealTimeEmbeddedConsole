/*******************************************************************************
 * Main file
 *
 * This module initialize all applications.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/
#include <stdio.h>

#include "interrupt.h"
#include "kernel.h"
#include "thermometer.h"
#include "chronometer.h"
#include "view_home.h"
#include "utils.h"
#include "aitc.h"
#include "exception.h"
#include "gpio.h"
#include "gpt.h"
#include "i2c.h"
#include "display.h"
#include "view_shell.h"
#include "shell_interpreter.h"
#include "serial.h"
#include "view_calibrate.h"
#include "calibrate.h"
#include "touchmanager.h"
#include "tsc2101.h"
#include "shell_program.h"
#include "controller_home.h"
#include "network.h"

/**
 * Initialize the hardware part.
 */
static void hardware_init() {

	interrupt_init();

	aitc_init();      // Must be after interrupt_init
	exception_init(); // Must be after interrupt_init

	gpio_init();      // Must be after aitc_init
	serial_init();    // Must be after aitc_init

	gpt_init();
	i2c_init();
	display_init();
	tsc2101_init();

	i2c_set_bus_speed(100000); // Must be after i2c_init
	serial_set_baudrate(9600); // Must be after serial_init
}

/**
 * Initialize the sofware part.
 */
static void sofware_init() {
	kernel_init();
	thermometer_init();
	chronometer_init();
	shell_interpreter_init();
	shell_program_init();
	touchmanager_init();
	network_init();
}
/**
 * Start the default threads
 */
static void startup_threads_start(){
	thermometer_thread_start();
	chronometer_thread_start();
	touchmanager_thread_start();
	shell_interpreter_start();
}

/**
 * Main application.
 *
 * @return exit status
 */
int main()
{
	hardware_init();
	sofware_init();
	startup_threads_start();

	controller_home_start();

	kernel_launch();
	return 0;
}
