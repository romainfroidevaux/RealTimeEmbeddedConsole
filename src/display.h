/*******************************************************************************
 * FPGA-GPIO Handling - Header File
 *
 * This module provides services to display a value on the FPGA 7-segment.
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

/**
 * Initialize the resource of the 7-Segment display module
 */
extern void display_init();

/**
 * Enable the refresh of the display.
 */
extern void display_enable();

/**
 * Disable the refresh of the display.
 */
extern void display_disable();

/**
 * Set the value to display.
 *
 * @param value The value to display, range from -99 to 99.
 */
extern void display_set(int8_t value);

#endif

