/*******************************************************************************
 * LCD Display - Header file
 *
 * This module provide an API to use the LCD display driver
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 *
 ******************************************************************************/

#pragma once
#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <stdbool.h>
#include "utils.h"
#include "view.h"
#include "xpm.h"

/**
 * Initialize the LCD display
 *
 * First call of the API, to initialize the LCD display
 */
extern void lcddisplay_init();

/**
 * Print a gridstruct utils_position start_pos, enum view_color color, char* string
 *
 * Print a grid on the LCD screen
 *
 * @param color the color of the grid
 * @param size the size of the grid
 */
extern void lcddisplay_print_grid(enum view_color color, enum view_grid_size size);

/**
 * Print an image
 *
 * Print an image on the LCD screen
 *
 * @param start_pos the high-left position of the image
 * @param icon structure of the XPM image
 */
extern void lcddisplay_print_image(struct utils_position start_pos, struct xpm_image icon);

/**
 * Print text
 *
 * Print a string text in the display
 *
 * @param string array of chars, string text to display
 * @param start_pos high-left position of the first char
 * @param color color of the text
 * @param whiteBackground color background in white if set; else, don't overwrite current pixel color
 */
extern void lcddisplay_print_text(char* string, struct utils_position start_pos, enum view_color color, bool whiteBackground);

/**
 * Print char
 *
 * Print just one char in the display
 *
 * @param c char to display
 * @param start_pos high-left position of the first char
 * @param color color of the char
 * @param whiteBackground color background in white if set; else, don't overwrite current pixel color
 */
extern void lcddisplay_print_char(char c, struct utils_position start_pos, enum view_color color, bool whiteBackground);

/**
 * Print zone
 *
 * Print a colored zone (rectangular)
 *
 * @param start_pos high-left position of the zone
 * @param stop_pos bottom-right position of the zone
 * @param color color og the zone
 */
extern void lcddisplay_print_zone(struct utils_position start_pos, struct utils_position stop_pos, enum view_color color);

/**
 * Swap LCD display
 *
 * Swap the screen memory area
 */
extern void lcddisplay_swap();

#endif
