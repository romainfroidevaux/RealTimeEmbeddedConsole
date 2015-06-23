/*******************************************************************************
 * Controller for Tic-Tac-Toe Module - Header file
 *
 * This module provide services for the Tic-Tac-Toe application.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef CONTROLLER_TICTACTOE_H
#define CONTROLLER_TICTACTOE_H
#include "utils.h"

/**
 * Validate the case chosen.
 *
 * Validate the case chosen and let the other player play.
 *
 * @param param : unused param
 */
extern void controller_tictactoe_validate_button_handler(void * param);

/**
 * Surrend the game.
 *
 * Give up the game and let the victory to the other player
 *
 * @param param : unused param
 */
extern void controller_tictactoe_surrender_button_handler(void * param);

/**
 * Handle the events on the tictactoe grid.
 *
 * Handle the events on the tictactoe grid. Update grid.
 *
 * @param p : position in the grid clicked 
 * @param param : unused param
 */
extern void controller_tictactoe_grid_handler(struct utils_position p, void * param);

/**
 * Starts the Tic-Tac-Toe app
 *
 */
extern void controller_tictactoe_start();

/**
 * Stops the Tic-Tac-Toe app
 *
 */
extern void controller_tictactoe_stop();

#endif
