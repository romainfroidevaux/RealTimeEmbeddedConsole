/*******************************************************************************
 * View Module - Header file
 *
 * This module provide the main view controller
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef VIEW_H
#define VIEW_H

/**
 * The different buttons of the main view
 */
enum view_button {
	home	//!< Return to home button
};

/**
 * The different colors
 */
enum view_color {
	red = 58051,	//!< Red color
	blue = 445, 	//!< Blue color
	green = 1928,	//!< Green color
	black = 0,		//!< Black color
	white = 65535,	//!< White color
	yellow = 65395	//!< Yellow color
};

/**
 * The different grid sizes
 */
enum view_grid_size {
	x33,	//!< 3x3 grid
	x66		//!< 6x6 grid
};

/**
 * Initialize view
 *
 * Initialize the global view with basics elements
 */
extern void view_init();

/**
 * Detach view
 *
 * Detach all elements of the the global view with basics elements
 */
extern void view_detach();

/**
 * Swap view
 *
 * Swap the view for Battleship game (Gamer1 to Gamer2 and return)
 */
extern void view_swap();

/**
 * Print Base
 *
 * Print commons elements for all views
 */
extern void view_print_base();

#endif
