/*******************************************************************************
 * 7-Segment Display Manager - Header file
 *
 * This module provide services for the 7-segment display.
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

#include "utils.h";
#include <stdint.h>

/**
 * Initialize the resource for the 7-Segment Display Manager
 */
extern void seg7display_init();

/**
 * Enable the refresh of the display.
 */
extern void seg7display_enable();

/**
 * Disable the refresh of the display.
 */
extern void seg7display_disable();

/**
 * Set the value to display.
 *
 * @param value The value to display, range from -99 to 99.
 */
extern void seg7display_set(int8_t value);

#endif
