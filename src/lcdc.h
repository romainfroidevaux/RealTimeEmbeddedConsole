/*******************************************************************************
 * LCLD - Liquid Crystal Display Controller
 *
 * Module to deal with the Liquid Crystal Display Controller (LCDC) of the
 * i.MX27.
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef LCDC_H
#define LCDC_H
/**
 * EIA-FR - Embedded Systems 1 laboratory
 *
 * Abstract:	LCLD - Liquid Crystal Display Controller
 *
 * Purpose:	Module to deal with the Liquid Crystal Display Controller
 * 		(LCDC) of the i.MX27.
 *
 * Author: 	Daniel Gachet
 * Update :	Romain Froidevaux
 * Date: 	12.12.2013
 */

#include <stdint.h>

/* LCD details (LW700AT) */
#define LCD_WIDTH 	800
#define LCD_HEIGHT 	480
#define LCD_FREQ	33260000 /* Hz */
#define LCD_BPP		16


/**
 * Initialize the liquid crystal display controller
 */
extern void lcdc_init();


/**
 * Enable the liquid crystal display controller
 */
extern void lcdc_enable();

/**
 * Swap the bitmap memory space address
 * Updated by R. Froidevaux
 */
extern void lcdc_memory_space_swap();


/**
 * Disable the liquid crystal display controller
 */
extern void lcdc_disable();


/**
 * Clear the background screen
 */
extern void lcdc_clear_screen ();


/**
 * Return the base address of the LCDC bitmap 
 */
extern uint16_t* lcdc_get_bitmap();


/**
 * Return the base address of the LCDC monitor 
 */
extern uint16_t* lcdc_get_monitor();


/**
 * Define the position of the monitor on the screen
 *
 * @param x horizontal starting position
 * @param y vertical starting position
 * @param width number of pixels in the width
 * @param height number of pixels in the height
 */
extern void lcdc_define_monitor_position (uint16_t x, uint16_t y, uint16_t width, uint16_t height);



#endif

