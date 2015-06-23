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

#include <stdlib.h>
#include <stdio.h>

#include "controller_battleship.h"
#include "view_battleship.h"
#include "model_battleship.h"
#include "model.h"
#include "network.h"
#include "ip.h"
#include "view_score.h"
#include "consolemanager.h"
#include "view_battleship_wait.h"
#include "shell_interpreter.h"
#include "model.h"
#include "model_score.h"
#include "utils.h"

// Constantes and local variables
#define BATTLESHIP_PORT 62234
#define BATTLESHIP_CMD_LEN 4
#define BATTLESHIP_UNDEFINED 10
static struct utils_position ship_selected_case = {BATTLESHIP_UNDEFINED, BATTLESHIP_UNDEFINED};
static int battleship_turn = model_player1;
static int battleship_notturn = model_player2;
static enum model_battleship_shipid selectedship;
struct utils_position p_selected;
static enum model_player player_playing = model_player1;
enum model_battleship_sens shipsens = horizontal;
enum model_battleship_view {
	positionning, waiting_player, playing, waiting_player2
};
enum model_battleship_view view;

// Static function called by shell
static void controller_battleship_shell_place_ship(int argc, char *argv[]) {

	if (argc != 5) {
		consolemanager_println("Syntax error. \n\rhelp battleship_place_ship");
		return;
	}

	struct utils_position p;
	p.x = atoi(argv[1]);
	p.y = atoi(argv[2]);
	int sens = argv[4][0] == 0 ? horizontal : vertical;
	if (argv[0][0] == '1') {
		model_battleship_place_ship(model_player1, atoi(argv[3]), p, sens);
	} else if (argv[0][0] == '2') {
		model_battleship_place_ship(model_player2, atoi(argv[3]), p, sens);
	} else {
		consolemanager_println("Syntax error.\n\r help battleship_place_ship");
		return;
	}
}

static void controller_battleship_shell_surrender(int argc,
		char *argv[] __attribute__((unused))) {
	if(argc == 1){
		controller_battleship_surrender_button_handler((void *) model_player2);
	}else if(argc == 0){
		controller_battleship_surrender_button_handler((void *) model_player1);
	}else{
		consolemanager_println("Syntax error.\n\r help battleship_surrender");
		return;
	}


}

static void controller_battleship_shell_shot_case(int argc, char *argv[]) {
	if (argc != 2) {
		consolemanager_println("Synthax error.\n\r help battleship_shot_case");
		return;
	}
	struct utils_position p;
	p.x = atoi(argv[1]);
	p.y = atoi(argv[2]);
	if (battleship_turn == model_player1) {
		if (model_battleship_get_case(model_player2, p) == ship_empty) {
			battleship_turn = model_player2;
			battleship_notturn = model_player1;
			model_battleship_add_case(model_player2, p, ship_miss);
			consolemanager_println("p1 shot successful");
			view = waiting_player2;
			view_battleship_swap();
		}
	} else {
		if (model_battleship_get_case(model_player1, p) == ship_empty) {
			battleship_turn = model_player2;
			battleship_notturn = model_player1;
			model_battleship_add_case(model_player1, p, ship_miss);
			consolemanager_println("p2 shot successful");
			view = playing;
			view_battleship_swap();
		}
	}
}

static void controller_battleship_shell_start(int argc, UNUSED(char *argv[])) {
	if(argc != 0){
		return;
	}

	if(model_battleship_all_ship_placed() && view == waiting_player){
		battleship_turn = model_player1;
		view = playing;
	}
}
// Shell commands and helpers
static cmd_t bs_cmd[] = { { .name = "battleship_place_ship", .cmd_function =
		controller_battleship_shell_place_ship, .help =
		"Place a ship on grid.\n\r"
				"Syntax: battleship_place_ship player player(1|2) x y id_ship(0-2) sens(0=horizontal|1=vertical)" },
		{ .name = "battleship_shot_case", .cmd_function =
				controller_battleship_shell_shot_case, .help =
				"Shot a case.\n\rSyntax: battleship_shot_case x y " }, {
				.name = "battleship_surrender", .cmd_function =
						controller_battleship_shell_surrender, .help =
						"Surrender the game.\n\rSyntax: battleship_surrender" },
		{ .name = "battleship_start", .cmd_function =
				controller_battleship_shell_start, .help =
				"Start game.\n\rSyntax: battleship_start x" }, };

// Controllers

void controller_battleship_button_ship1_handler(
		void * param __attribute__((unused))) {
	selectedship = submarine;
	consolemanager_println("Ship submarine (1) selected");
}

void controller_battleship_button_ship2_handler(
		void * param __attribute__((unused))) {
	selectedship = destroyer;
	consolemanager_println("Ship destroyer (2) selected");
}

void controller_battleship_button_ship3_handler(
		void * param __attribute__((unused))) {
	selectedship = aircraft_carrier;
	consolemanager_println("Ship aircraft_carrier (3) selected");
}

void controller_battleship_button_rotate_handler(
		void * param __attribute__((unused))) {
	if (shipsens == horizontal) {
		shipsens = vertical;
		consolemanager_println("Vertical");
	} else {
		shipsens = horizontal;
		consolemanager_println("Horizontal");
	}

}

void controller_battleship_grid_handler(struct utils_position position,
		void * UNUSED(param)) {
	// Positionning
	if (view == positionning) {
		if (selectedship != none) {
			model_battleship_place_ship(player_playing, selectedship, position,
					shipsens);
			struct utils_position p;
			int size = (selectedship == aircraft_carrier ) ? 4 : (selectedship == destroyer) ? 3 : 2;
			if(shipsens == vertical){
				for(int i = 0;i<size;i++){
					p.x = position.x;
					p.y = position.y + i;
					model_battleship_add_case(model_player1, p, ship_present);
					view_battleship_set_case(p, ship_present);
				}
			}else{

				for(int i = 0;i<size;i++){
					p.x = position.x + i;
					p.y = position.y;
					model_battleship_add_case(model_player1, p, ship_present);
					view_battleship_set_case(p, ship_present);
				}
			}
		}
		selectedship = none;

		// Playing
	} else {
		if (view == playing) {
			if (p_selected.x != BATTLESHIP_UNDEFINED) {
				view_battleship_set_case(p_selected,
						model_battleship_get_case(model_player1, p_selected));
			}
			p_selected = position;
			view_battleship_set_case(p_selected, ship_select);
			view = waiting_player2;
		}
	}

}

void controller_battleship_play_validate_button_handler(
		void * param __attribute__((unused))) {
	if (view == positionning) {
		if (model_battleship_all_ship_placed()) {
			// Waiting other player
			view_battleship_wait_init();
			model_battleship_init_game_grid();
			view = waiting_player;
			//struct ip_addr ip_broadcast = {255,255,255,255};
			//network_send_udp_paquet(ip_broadcast, BATTLESHIP_PORT, BATTLESHIP_PORT, data, len);

			//struct battleship_data data = network_wait_packet(ip_broadcast, BATTLESHIP_PORT, BATTLESHIP_PORT);
			//view_battleship_set_case(position, case_type);
		}
		consolemanager_print(
				"You need to place all ships before you can start the game.");
	} else if (view == playing) {
		// Shot
		int sel_case_state = model_battleship_get_case(model_player2, ship_selected_case);
		if(sel_case_state == ship_empty){
			model_battleship_add_case(model_player1, ship_selected_case, ship_miss);
			view_battleship_set_case(ship_selected_case, ship_miss);
			battleship_turn = model_player2;
			battleship_notturn = model_player1;
		}else if(sel_case_state == ship_present){
			model_battleship_add_case(model_player1, ship_selected_case, ship_hit);
			view_battleship_set_case(ship_selected_case, ship_hit);
			battleship_turn = model_player2;
			battleship_notturn = model_player1;
		}
	}

}

void controller_battleship_surrender_button_handler(
		void * param) {
	if(param == 0){
		model_score_set_winner(model_player2);
	}else{
		model_score_set_winner(model_player1);
	}
	controller_battleship_stop();

	//network_send_udp_paquet(ip_advers, BATTLESHIP_PORT, BATTLESHIP_PORT,
	//"battleship_surrender");
	//TODO : Send surrender packet
}

void controller_battleship_start() {

	// Attach all bs commands
	for (int i = 0; i < BATTLESHIP_CMD_LEN; i++) {
		shell_interpreter_attach(&bs_cmd[i]);
	}

	model_battleship_reset_grid();
	selectedship = none;
	view = positionning;
	model_start_app(app_battleship);
	view_battleship_init();
}

void controller_battleship_stop() {
	model_stop_app(app_battleship);
	for (int i = 0; i < BATTLESHIP_CMD_LEN; i++) {
		shell_interpreter_detach(&bs_cmd[i]);
	}
	view_score_init(model_score_get_score(model_player1),
			model_score_get_score(model_player2), score_replay);
}

