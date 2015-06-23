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

#pragma once
#ifndef THERMOMETER_H
#define THERMOMETER_H

#include "utils.h"

/**
 * Initialize the Thermometer resources.
 */
extern void thermometer_init();

/**
 * Start the Thermometer application.
 */
extern void thermometer_thread_start();

/**
 * Stop the Thermometer application.
 */
extern void thermometer_thread_stop();

/**
 * Stop the refresh display thread.
 */
extern void thermometer_display_thread_stop();

/**
 * Start the refresh display thread.
 */
extern void thermometer_display_thread_start();


#endif
