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

#include <stdbool.h>
#include "model_battleship.h"
#include "view_battleship.h"
#include "utils.h"
#include "model_score.h"
#include "view_battleship_play.h"

#define BATTLESHIP_SIZE 6
#define BATTLESHIP_NB_SHIP 3
#define BATTLESHIP_NB_PLAYER 2
#define BATTLESHIP_UNDEFINED 10

struct battleship_grid {
	enum view_battleship_case grid[BATTLESHIP_SIZE][BATTLESHIP_SIZE];
};
struct battleship_grid player1Grid;
struct battleship_grid player2Grid;
typedef struct battleship_ship ship_t;
struct battleship_ship {
	struct utils_position p;
	enum model_battleship_sens sens;
	int size;
};

static ship_t all_ships[BATTLESHIP_NB_PLAYER][BATTLESHIP_NB_SHIP];

int model_battleship_valid(struct utils_position p) {
	return (p.x >= 0 && p.x < BATTLESHIP_SIZE && p.y >= 0
			&& p.y < BATTLESHIP_SIZE);
}

int model_battleship_add_case(enum model_player player,
		struct utils_position position, enum view_battleship_case case_type) {
	if (model_battleship_valid(position)) {
		if (player == model_player1) {
			player1Grid.grid[position.x][position.y] = case_type;
		} else {
			player2Grid.grid[position.x][position.y] = case_type;
		}
	}
	return 0;
}

int model_battleship_winner() {
	int j1 = 0;
	int j2 = 0;
	for (int i = 0; i < BATTLESHIP_NB_PLAYER; i++) {
		for (int j = 0; j < BATTLESHIP_NB_SHIP; j++) {
			ship_t current_ship = all_ships[i][j];
			if (current_ship.sens == vertical) {
				for (int y = current_ship.p.x; y < current_ship.size; y++) {
					if (player1Grid.grid[current_ship.p.x][y] == ship_hit) {
						j1++;
					}
					if (player2Grid.grid[current_ship.p.x][y] == ship_hit) {
						j2++;
					}
				}
			}
		}
	}
	if (j1 == 8) {
		return score_player1;
	}
	if (j2 == 8) {
		return score_player2;
	}
	return score_tie;
}

void model_battleship_reset_grid() {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			player1Grid.grid[i][j] = ship_empty;
			player2Grid.grid[i][j] = ship_empty;
		}
	}
	for (int i = 0; i < BATTLESHIP_NB_PLAYER; i++) {
		for (int j = 0; j < BATTLESHIP_NB_SHIP; j++) {
			all_ships[i][j].p.x = BATTLESHIP_UNDEFINED;
			all_ships[i][j].p.y = BATTLESHIP_UNDEFINED;
		}
	}
}

bool model_battleship_place_ship(enum model_player player,
		enum model_battleship_shipid ship, struct utils_position position,
		enum model_battleship_sens sens) {
	all_ships[player][ship].p = position;
	all_ships[player][ship].sens = sens;
	return true;
}

bool model_battleship_all_ship_placed() {
	for (int i = 0; i < BATTLESHIP_NB_PLAYER; i++) {
		for (int j = 0; j < BATTLESHIP_NB_SHIP; j++) {
			if (all_ships[i][j].p.x == BATTLESHIP_UNDEFINED) {
				return false;
			}
		}
	}
	return true;
}

int model_battleship_get_case(enum model_player player, struct utils_position p) {
	if (model_battleship_valid(p)) {
		if (player == model_player1)
			return player1Grid.grid[p.x][p.y];
		else {
			return player2Grid.grid[p.x][p.y];
		}
	}
	return -1;
}

void model_battleship_init(){
	all_ships[model_player1][submarine].size = BATTLESHIP_SUBMARINE_SIZE;
	all_ships[model_player1][destroyer].size = BATTLESHIP_DESTROYER_SIZE;
	all_ships[model_player1][aircraft_carrier].size = BATTLESHIP_AIRCRAFT_CARRIER_SIZE;
	all_ships[model_player2][submarine].size = BATTLESHIP_SUBMARINE_SIZE;
	all_ships[model_player2][destroyer].size = BATTLESHIP_DESTROYER_SIZE;
	all_ships[model_player2][aircraft_carrier].size = BATTLESHIP_AIRCRAFT_CARRIER_SIZE;
}

void model_battleship_init_game_grid(){
	for (int i = 0; i < BATTLESHIP_NB_PLAYER; i++) {
		for (int j = 0; j < BATTLESHIP_NB_SHIP; j++) {
			for(int k = 0;k<all_ships[i][j].size;k++){

			}

		}
	}
}
