/*******************************************************************************
 * Chronometer Application - Header file
 *
 * This is the application for the chronometer.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef CHRONOMETER_H
#define CHRONOMETER_H

#include "utils.h"

/**
 * Get the time value of the chronometer
 * @return the time
 */
struct utils_time chronometer_get_time();

/**
 * Initialize the chronometer resources.
 */
extern void chronometer_init();

/**
 * Start the chronometer application.
 */
extern int chronometer_thread_start();

/**
 * Stop the chronometer application.
 */
extern void chronometer_thread_stop();

/**
 * Get the time of the chronometer.
 *
 * @return the time
 */
extern struct utils_time chronometer_get_time();

/**
 * Start the chronometer counter.
 */
extern void chronometer_start();

/**
 * Stop the chronometer counter.
 *
 * The chronometer is no more incremented.
 */
extern void chronometer_stop();

/**
 * Reset the chronometer counter.
 *
 * The counter is reset to 0 and the counter is stopped.
 */
extern void chronometer_reset();

#endif
