/*******************************************************************************
 * Controller for Home Module - Header file
 *
 * This module provide services for Home view.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef CONTROLLER_HOME_H
#define CONTROLLER_HOME_H

/**
 * Starts the battleship app.
 *
 * Starts the battleship game interface.
 *
 * @param param : unused param.
 */
extern void controller_home_button_battleship_handler(void * param);

/**
 * Starts the Tic-tac-toe app.
 *
 * Starts the Tic-tac-toe game interface.
 *
 * @param param : unused param.
 */
extern void controller_home_button_tictactoe_handler(void * param);

/**
 *  Starts the score app
 *
 *  Shows the score view on screen.
 *
 * @param param : unused param.
 */
extern void controller_home_button_score_handler(void * param);

/**
 * Starts the shell app.
 *
 * Shows the shell view and console on screen.
 *
 * @param param : unused param.
 */
extern void controller_home_button_shell_handler(void * param);

/**
 * Starts the home app
 *
 */
extern void controller_home_start();

/**
 * Stops the home app
 *
 */
extern void controller_home_stop();

#endif
