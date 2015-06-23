/*******************************************************************************
 * Battleship Play View Module - Header file
 *
 * This module provide the view of play phase of the Battleship game
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef VIEW_BATTLESHIP_PLAY_H
#define VIEW_BATTLESHIP_PLAY_H

#include "view_battleship.h"

/**
 * The different buttons of the view
 */
enum view_battleship_play_button {
	validate	//!< Validate the ship position
};

/**
 * Initialize Battleship Play view
 *
 * Initialize the Battleship Play view with basics elements
 */
extern void view_battleship_play_init();

/**
 * Detach all elements of Battleship Play view
 *
 * Detach all elements of the Battleship Play view with basics elements
 */
extern void view_battleship_play_detach();

/**
 * Set score
 *
 * Set the score (hit and missed points)
 * @param hit counter of hits for the current play
 * @param missed counter of missed for the current play
 */
extern void view_battleship_play_set_score(uint8_t hit, uint8_t missed);

/**
 * Set validate button state
 *
 * Set the state of the validate button
 * @param state the state of the button
 */
extern void view_battleship_play_set_validate_button_state(enum utils_button_state state);

#endif
