/*******************************************************************************
 * Controller for Score Module - Header file
 *
 * This module provide services for the Score module.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdio.h>
#include "model.h"
#include "controller_score.h"
#include "model_score.h"
#include "view_score.h"
#include "view_home.h"
#include "controller_tictactoe.h"
#include "consolemanager.h"
#include "controller_battleship.h"
#include "controller_home.h"

void controller_score_button_replay_handler(
		void * param __attribute__ ((unused))) {
	enum model_application app = model_get_last_game();
	if (app == app_tictactoe){
		model_stop_app(app_scores);
		controller_tictactoe_start();
	}
	if(app == app_battleship) {
		model_stop_app(app_scores);
		controller_battleship_start();
	}
}

void controller_score_button_reset_handler(
		void * param __attribute__ ((unused))) {
	if (model_get_last_game() == app_home) {
		model_score_reset();
		view_score_values_print(0, 0);
	}
}

void controller_score_start(enum view_score_button_type type) {
	model_start_app(app_scores);
	model_set_current_interface(app_scores);

	view_score_init(model_score_get_score(model_player1),
			model_score_get_score(model_player2), type);
}

void controller_score_stop() {
	model_stop_app(app_scores);
	controller_home_start();
}
