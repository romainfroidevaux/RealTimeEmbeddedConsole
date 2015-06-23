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
#ifndef CONTROLLER_BATTLESHIP_H
#define CONTROLLER_BATTLESHIP_H

#include "utils.h"

/**
 * Choose the first ship to place it on grid.
 *
 * @param param : unused param
 */
extern void controller_battleship_button_ship1_handler(void * param);

/**
 * Choose the second ship to place it on grid.
 *
 * @param param : unused param
 */
extern void controller_battleship_button_ship2_handler(void * param);

/**
 * Choose the third ship to place it on grid.
 *
 * @param param : unused param
 */
extern void controller_battleship_button_ship3_handler(void * param);

/**
 * Rotate the position of the current selected ship.
 *
 * @param param : unused param
 */
extern void controller_battleship_button_rotate_handler(void * param);

/**
 * Handle the button events on the battleship grid.
 *
 * @param position : position in the grid clicked (struct utils_position)
 * @param param : unused param
 */
extern void controller_battleship_grid_handler(struct utils_position position, void * param __attribute__((unused)));


/**
 * Handle the button events on the battleship view.
 *
 * Handle the button events on the battleship_play. Update the grid and go to next interface.
 *
 * @param param : unused param
 */
extern void controller_battleship_play_validate_button_handler(void * param);

/**
 * Surrend the game.
 *
 * Give up the game and let the victory to the other player
 *
 * @param param : unused param
 */
extern void controller_battleship_surrender_button_handler(void * param);

/**
 * Starts the battleship app
 *
 */
extern void controller_battleship_start();

/**
 * Stops the battleship app
 *
 */
extern void controller_battleship_stop();

#endif



