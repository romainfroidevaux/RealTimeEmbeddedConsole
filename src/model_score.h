/*******************************************************************************
 * Model for Score Module - Header file
 *
 * This module provide services for the Score module.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef MODEL_SCORE_H
#define MODEL_SCORE_H
#include "model.h"

/**
 * End game state
 */
enum model_score_endgame{
	score_player1, //!< Player 1 won
	score_player2, //!< Player 2 won
	score_tie		 //!< Tie
};

/**
 * Get the current score for the selected player.
 *
 * Get the current score for the selected player.
 *
 * @param player : The player id
 * @return score for the player
 */
extern int model_score_get_score(enum model_player player);

/**
 * Updates the score.
 *
 * Updates the score for the player.
 *
 * @param player : The player id
 */
extern void model_score_set_winner(enum model_player player);

/**
 * Resets the score for all players.
 */
extern void model_score_reset();

#endif
