/*******************************************************************************
 * Model for the global Application - Header file
 *
 * This module provide services for the Application.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "utils.h"

/**
 * Type of application running
 */
enum model_application {
	app_battleship = 0,  //!< Battleship app
	app_tictactoe = 1,   //!< Tic-Tac-Toe app
	app_shell = 2, 		 //!< Shell app
	app_home = 3, 		 //!< The home screen
	app_scores = 4,      //!< The Score screen
	app_chronometre = 5, //!< The chronometre
	app_thermometre = 6,  //!< The thermometre
	app_calibrate = 7,    //!< The screen calibration program
	app_touchmanager = 8, //!<The touchmanager
	app_shellinterpreter = 9, //!< The shell interpreter
	app_nb = 10           //!< The size of this enum
};

/**
 * Player
 */
enum model_player {
	model_player1 = 0,
	model_player2 = 1
};

/**
 * Gives a list of the app.
 *
 * @param state : filters the app (all, running, stopped)
 * @return app_list : String with app_list
 */
extern char* model_get_app_list(enum utils_app_state state);

/**
 * Starts an application.
 *
 * Informs the model that we started an application.
 *
 * @param app : application started
 */
extern void model_start_app(enum model_application app);

/**
 * Stops an application.
 *
 * Informs the model that we stopped an application.
 *
 * @param app : application stopped
 */
extern void model_stop_app(enum model_application app);

/**
 * Get last game running.
 *
 * @return : application started
 */
extern int model_get_last_game();

/**
 * Set last game running.
 *
 * @param app : application started
 */
extern void  model_set_last_game(enum model_application app);

/**
 * Set current running app.
 *
 * @param app : application running on LCD screen
 */
extern void model_set_current_interface(enum model_application app);

/**
 * Get current running app.
 *
 * @return app : application running on LCD screen
 */
extern int model_get_current_interface();


#endif
