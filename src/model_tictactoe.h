/*******************************************************************************
 * Controller for Battleship Module - Header file
 *
 * This module provide services for the controller module.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef MODEL_TICTACTOE_H
#define MODEL_TICTACTOE_H
#include <stdbool.h>
#include "view_tictactoe.h"
#include "utils.h"
#include "model.h"
#include "model_score.h"

/**
 * Returns true if the game has ended.
 *
 * @return is_end : true if the game has ended
 */
extern bool model_tictactoe_is_end();

/**
 * Update the grid to add the last move.
 *
 * @param player : The player id
 * @param position : The position on the grid
 * @param case_type : The type of case it is
 */
extern int model_tictactoe_add_case(struct utils_position position,
		enum view_tictactoe_case case_type);

/**
 * Returns the winner or sets it when surrender.
 *
 * Returns the winner id or tie if nobody won or sets it when surrender.
 *
 * @return model_score_endgame : winner
 */
extern int model_tictactoe_winner(struct utils_position p,
		int s);

/**
 * Resets the grid.
 *
 * Resets the Tic-Tac-Toe grid.
 *
 */
extern void model_tictactoe_reset_grid();

/**
 * Get the grid case state.
 *
 * @param position in the grid
 * @return the state
 */
extern int model_tictactoe_get_case(struct utils_position position);

#endif
