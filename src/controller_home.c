/*******************************************************************************
 * Controller for Home Module - Header file
 *
 * This module provide services for Home view.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include "model.h"
#include "view_score.h"
#include "controller_home.h"
#include "controller_battleship.h"
#include "controller_score.h"
#include "controller_tictactoe.h"
#include "shell_interpreter.h"
#include "view_home.h"
#include "view_shell.h"
#include "thermometer.h"


void controller_home_button_battleship_handler(void * param __attribute__ ((unused))){

	controller_home_stop();
	model_set_last_game(app_battleship);
	controller_battleship_start();
}

void controller_home_button_tictactoe_handler(void * UNUSED(param)){
	controller_home_stop();
	model_set_last_game(app_tictactoe);
	controller_tictactoe_start();
}

void controller_home_button_score_handler(void * UNUSED(param)){
	controller_home_stop();
	controller_score_start(score_reset);
}

void controller_home_button_shell_handler(void * UNUSED(param)){
	controller_home_stop();
	model_start_app(app_shell);
	view_shell_init();
}

void controller_home_start(){
	model_start_app(app_home);
	view_home_init();
	thermometer_display_thread_start();
}

void controller_home_stop(){
	model_stop_app(app_home);
	thermometer_display_thread_stop();
}

