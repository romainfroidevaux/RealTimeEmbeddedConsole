/*******************************************************************************
 * TSC2101 - Touch Screen Controller
 *
 * Module to deal with the TSC2101 Controller interfacing the touchscreen of the
 * APF27 module.
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef TSC2101_H
#define TSC2101_H

#include <stdint.h>

/**
 * Finger postion on the touch screen
 */
struct tsc2101_position {
	uint16_t x;
	uint16_t y;
	uint16_t z;
};


/**
 * Method to initialize the resources of the TSC2101 controller 
 * This method should be called prior any other one.
 */
extern void tsc2101_init();


/**
 * Method to read the position of finger on the touch screen
 * @return finger position
 */
extern struct tsc2101_position tsc2101_read_position();

#endif

