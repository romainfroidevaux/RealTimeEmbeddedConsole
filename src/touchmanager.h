/*******************************************************************************
 * Touch Manager - Header file
 *
 * This module provide services for the application view.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef TOUCHMANAGER_H
#define TOUCHMANAGER_H

#include <stdint.h>
#include <stdbool.h>

#include "utils.h"

/**
 * Type for the callback routine when a button is pressed.
 *
 * @param The button id
 * @param param Any optional parameter
 */
typedef void (*touchmanager_button_routine_t)(void* param);

/**
 * Type for the callback routine when a case on the grid is pressed.
 *
 * @param The case position
 * @param param Any optional parameter
 */
typedef void (*touchmanager_grid_routine_t)(
		struct utils_position position,
		void* param);

/**
 * Initialize the Touch Manager.
 *
 * @return 0 on success, a negative number on error
 */
extern int touchmanager_init();

/**
 * Enable the touch manager application.
 */
extern void touchmanager_thread_start();

/**
 * Disable the touch manager application.
 */
extern void touchmanager_thread_stop();

/**
 * Get the status of the thread.
 *
 * @return true if the thread is running, false if not running
 */
extern bool touchmanager_is_running();

/**
 * Set the calibration config.
 *
 * @param config the calibration config
 */
extern void touchmanager_set_config(int config[7]);

extern int touchmanager_grid_attach(
		struct utils_position position,
		struct utils_size size,
		struct utils_size dimension,
		touchmanager_grid_routine_t routine,
		void* param);

/**
 * Detach a grid.
 *
 * @param index the index of the grid to detach
 * @return 0 on success, -1 on error
 */
extern int touchmanager_grid_detach(int index);

/**
 * Detach all grids.
 */
extern void touchmanager_grid_detach_all();

/**
 * Attach a new button.
 *
 * @param position the top left position of the button in pixel
 * @param size the size of the button in pixel
 * @param routine the callback routine
 * @param param the parameter to pass to the routine
 * @return the index of the button, -1 on error
 */
extern int touchmanager_button_attach(
		struct utils_position position,
		struct utils_size size,
		touchmanager_button_routine_t routine,
		void* param);

/**
 * Detach the button.
 *
 * @param index the index of the button to detach
 * @return 0 on success, -1 on error
 */
extern int touchmanager_button_detach(int index);

/**
 * Detach all buttons.
 */
extern void touchmanager_button_detach_all();

/**
 * Detach all touch surface.
 */
extern void touchmanager_detach_all();

/**
 * Get the position when the touchscreen is no more touched.
 *
 * @param calibrated true the position is calibrated, false the raw value of the position
 * @return the position
 */
extern struct utils_position touchmanager_wait_release(bool calibrated);

#endif
