/*******************************************************************************
 * Score View Module - Header file
 *
 * This module provide the view for the score screen
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdint.h>

#pragma once
#ifndef VIEW_SCORE_H
#define VIEW_SCORE_H

/**
 * The different buttons
 */
enum view_score_button {
	replay_reset	//!< The reset/replay button
};

/**
 * The different modes of button score
 */
enum view_score_button_type {
	score_replay,	//!< Use a replay button
	score_reset 	//!< Use a reset button
};

/**
 * Initialize view
 *
 * Initialize the score screen view with basics elements
 * @param player_score score of the player
 * @param enemy_score score of the enemy
 * @param button_type the type of the button
 */
extern void view_score_init(uint8_t player_score, uint8_t enemy_score, enum view_score_button_type button_type);

/**
 * Detach view
 *
 * Detach all elements of the score screen view with basics elements
 * @param player_score score of the player
 * @param enemy_score score of the enemy
 * @param button_type the type of the button
 */
extern void view_score_detach();

extern void view_score_values_print(uint8_t player_score, uint8_t enemy_score);



#endif
