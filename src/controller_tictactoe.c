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

#include <string.h>
#include <stdlib.h>

#include "controller_tictactoe.h"
#include "consolemanager.h"
#include "model.h"
#include "model_tictactoe.h"
#include "view_tictactoe.h"
#include "utils.h"
#include "model_score.h"
#include "view_score.h"
#include "shell_interpreter.h"

#define TICTACTOE_UNDEFINED 10

struct utils_position tictactoe_selected_case = { TICTACTOE_UNDEFINED,
TICTACTOE_UNDEFINED };
int tictactoe_turn = model_player1;

static void controller_tictactoe_startgame(int argc, UNUSED(char *argv[])) {
	if(argc == 0)
		view_tictactoe_set_turn_text(your);
}

static void controller_tictactoe_play(int argc, char *argv[]) {
	if (argc != 2) {
		consolemanager_println("Synthax error.");
		return;
	}
	struct utils_position p;
	p.x = atoi(argv[0]);
	p.y = atoi(argv[1]);
	tictactoe_selected_case = p;
	controller_tictactoe_validate_button_handler(0);
}

static void controller_tictactoe_surrender_shell_handler(int UNUSED(argc), char* UNUSED(argv[])) {
	if(argc == 1){
		controller_tictactoe_surrender_button_handler((void *) model_player2);
	}else if(argc == 0){
		controller_tictactoe_surrender_button_handler((void *) model_player1);
	}else{
		consolemanager_println("Syntax error.\n\r help tictactoe_surrender");
		return;
	}
}

static cmd_t ttt_cmd[] = { { .name = "tictactoe_start", .cmd_function =
		controller_tictactoe_startgame, .help = "Place a ship on grid.\n\r"
		"Syntax: battleship_place_ship player(1|2) x y id_ship sens" }, {
		.name = "tictactoe_play", .cmd_function = controller_tictactoe_play,
		.help = "Shot a case.\n\rSyntax: tictactoe_play x y " }, { .name =
		"tictactoe_surrender",
		.cmd_function = controller_tictactoe_surrender_shell_handler,
		.help = "Surrender the game.\n\rSyntax: tictactoe_surrender" } };

void controller_tictactoe_validate_button_handler(
		void * UNUSED(param)) {
	// Me
	if (tictactoe_turn == model_player1) {
		if (model_tictactoe_add_case(tictactoe_selected_case,
				circle_validate)) {
			view_tictactoe_set_case(tictactoe_selected_case, circle_validate);
			tictactoe_turn = model_player2;
			// TODO : Send UDP
		}
		// Other player
	} else {
		if (model_tictactoe_add_case(tictactoe_selected_case, cross_validate)) {
			view_tictactoe_set_case(tictactoe_selected_case, cross_validate);
			tictactoe_turn = model_player1;
			// TODO : Send UDP
		}
	}
	// In all cases, check if end game.
	int type = (tictactoe_turn == model_player2) ? circle_validate : cross_validate;
	if(model_tictactoe_winner(tictactoe_selected_case, type)){
		model_score_set_winner((tictactoe_turn == model_player1) ? model_player2 : model_player1);
				controller_tictactoe_stop();
	}
	// Check all cases full
	if (model_tictactoe_is_end()) {
		controller_tictactoe_stop();
	}
	// resets case
	tictactoe_selected_case.x = TICTACTOE_UNDEFINED;
	tictactoe_selected_case.y = TICTACTOE_UNDEFINED;
}

void controller_tictactoe_surrender_button_handler(
		void * UNUSED(param)) {
	if(param == model_player1){
		model_score_set_winner(score_player2);
	}else{
		model_score_set_winner(score_player1);
	}
	controller_tictactoe_stop();

}

void controller_tictactoe_grid_handler(struct utils_position p,
		void * UNUSED(param)) {
	// Me
	if (tictactoe_turn == model_player1) {
		if ((p.x >= 0) && (p.x < 3) && (p.y >= 0) && (p.y < 3)) {

			if(model_tictactoe_get_case(p) == tictactoe_empty){
				view_tictactoe_set_case(tictactoe_selected_case, tictactoe_empty);
				tictactoe_selected_case = p;
				view_tictactoe_set_case(p, circle_select);
			}

		}
	}
}

void controller_tictactoe_start() {
	model_tictactoe_reset_grid();
	tictactoe_selected_case.x  = TICTACTOE_UNDEFINED;
	tictactoe_selected_case.y = TICTACTOE_UNDEFINED;
	model_start_app(app_tictactoe);
	view_tictactoe_init();
	// Attach all ttt commands
	for (int i = 0; i < 3; i++) {
		shell_interpreter_attach(&ttt_cmd[i]);
	}
}

void controller_tictactoe_stop() {
	model_stop_app(app_tictactoe);
	// Detach all ttt commands
	for (int i = 0; i < 3; i++) {
		shell_interpreter_detach(&ttt_cmd[i]);
	}
	view_score_init(model_score_get_score(model_player1),
				model_score_get_score(model_player2), score_replay);
}

