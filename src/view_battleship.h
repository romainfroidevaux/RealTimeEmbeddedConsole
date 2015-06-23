/*******************************************************************************
 * Battleship Positioning View Module - Header file
 *
 * This module provide the view of positioning phase of the Battleship game
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef VIEW_BATTLESHIP_H
#define VIEW_BATTLESHIP_H

#include "utils.h"

/**
 * The different type of case
 */
enum view_battleship_case {
	ship_empty,		//!< Empty case
	ship_hit,		//!< Hit ship case
	ship_miss,		//!< Missed ship case
	ship_select,	//!< Selected ship case
	ship_present	//!< Present ship case
};

/**
 * The different buttons of the view
 */
enum view_battleship_button {
	ship1,		//!< Fist ship button
	ship2,		//!< Second ship button
	ship3,		//!< Third ship button
	rotate,		//!< Button to rotate ship
	play		//!< Button to be ready to play game
};

/**
 * Initialize Battleship Positioning view
 *
 * Initialize the Battleship Positioning view with basics elements
 */
extern void view_battleship_init();

/**
 * Detach Battleship view
 *
 * Detach all elements of the Battleship view with basics elements
 */
extern void view_battleship_detach();

/**
 * Set case
 *
 * Set grid case with different elements
 * @param position the position of the case into the grid
 * @param case_type the type of the case
 */
extern void view_battleship_set_case(struct utils_position position, enum view_battleship_case case_type);


/**
 * Set play button state
 *
 * Set the state of the play button
 * @param state the state of the button
 */
extern void view_battleship_set_play_button_state(enum utils_button_state state);

/**
 * Swap view
 *
 * Swap view to switch users screen
 */
extern void view_battleship_swap();

#endif
