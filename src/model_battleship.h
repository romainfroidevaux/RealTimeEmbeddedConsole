/*******************************************************************************
 * Model for Battleship Module - Header file
 *
 * This module provide services for the Battleship module.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef MODEL_BATTLESHIP_H
#define MODEL_BATTLESHIP_H

#include <stdbool.h>
#include "view_battleship.h"
#include "utils.h"
#include "model.h"

#define BATTLESHIP_SUBMARINE_SIZE 2
#define BATTLESHIP_DESTROYER_SIZE 3
#define BATTLESHIP_AIRCRAFT_CARRIER_SIZE 4

/**
 * Type of ship
 */
enum model_battleship_shipid {
	submarine = 0, destroyer = 1, aircraft_carrier = 2, none
};

/**
 * Sens of ship
 */
enum model_battleship_sens {
	horizontal = 0, vertical = 1
};

/**
 * Update the grid to add the last move.
 *
 * @param player : The player id
 * @param position : The position on the grid
 * @param case_type : The type of case it is 
 */
extern int model_battleship_add_case(enum model_player player,
		struct utils_position position, enum view_battleship_case case_type);

/**
 * Returns the winner id.
 *
 * @return model_score_endgame : winner
 */
extern int model_battleship_winner();

/**
 * Resets the grid.
 *
 * Resets the battleship grid.
 *
 */
extern void model_battleship_reset_grid();

/**
 * Place a ship on the grid.
 *
 * @param player : id player
 * @param ship : id of the ship placed
 * @param position : position of the ship
 * @param sens : horizontal or vertical
 * @return placable : return true if the ship was able to be placed on the grid.
 */
extern bool model_battleship_place_ship(enum model_player player,
		enum model_battleship_shipid ship, struct utils_position position,
		enum model_battleship_sens sens);

/**
 * Check if all ships placed
 *
 * @return true if all ships are placed
 */
extern bool model_battleship_all_ship_placed();

/**
 * Gets a case on the player grid.
 * @param player : player id
 * @param p : Position to get
 * @return : type of case in the player grid
 */
extern int model_battleship_get_case(enum model_player player, struct utils_position p);

/**
 * Init the size of the ships
 */
extern void model_battleship_init();
/**
 * Init the grid for a new game
 */
extern void model_battleship_init_game_grid();
#endif
