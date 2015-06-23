/*******************************************************************************
 * Home View Module - Header file
 *
 * This module provide the view for the home screen
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef VIEW_HOME_H
#define VIEW_HOME_H

#include "utils.h"

/**
 * The different buttons
 */
enum view_home_button {
	battleship,	//!< Battleship game button
	tictactoe,	//!< TicTacToe game button
	score,		//!< Score app button
	shell,		//!< Shell app button
	view_home_button_nb //!< Number of buttons
};

/**
 * Initialize view
 *
 * Initialize the home screen view with basics elements
 */
extern void view_home_init();

/**
 * Detach view
 *
 * Detach all elements of the home screen view with basics elements
 */
extern void view_home_detach();

/**
 * Set time
 *
 * Set the time value of the chronometer
 * @param time current time value of the chronometer
 */
extern void view_home_set_time(struct utils_time time);

/**
 * Set temprature
 *
 * Set the temperature value of the LM75
 * @param temperature current temperature of the LM75 thermometer
 */
extern void view_home_set_temperature(struct utils_temperature temperature);

#endif
