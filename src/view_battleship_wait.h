/*******************************************************************************
 * Battleship Waiting View Module - Header file
 *
 * This module provide the view of waiting phase of the Battleship game
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef VIEW_BATTLESHIP_WAIT_H
#define VIEW_BATTLESHIP_WAIT_H

#include "view_battleship.h"

/**
 * Initialize Battleship Wait view
 *
 * Initialize the Battleship Wait view with basics elements
 */
extern void view_battleship_wait_init();

/**
 * Detach Battleship Play view
 *
 * Detach all elements of the Battleship Play view with basics elements
 */
extern void view_battleship_wait_detach();

/**
 * Set score
 *
 * Set the score (hit and missed points)
 * @param hit enemy counter of hits for the current play
 * @param missed enemy counter of missed for the current play
 */
extern void view_battleship_wait_set_score(uint8_t hit, uint8_t missed);

#endif
