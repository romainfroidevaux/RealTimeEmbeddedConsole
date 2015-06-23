/*******************************************************************************
 * TicTacToe View Module - Header file
 *
 * This module provide the view of the TicTacToe game
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef VIEW_TICTACTOE_H
#define VIEW_TICTACTOE_H

#include "utils.h"

/**
 * The different type of case
 */
enum view_tictactoe_case {
	tictactoe_empty,	//!< Empty case
	circle_validate,	//!< Validated circle case
	circle_select,		//!< Selected circle case
	cross_validate,		//!< Validated cross case
	cross_select		//!< Selected cross case
};

/**
 * The different buttons of the view
 */
enum view_tictactoe_button {
	validate_btn	//!< Validate button
};

/**
 * The different turns of the game
 */
enum view_tictactoe_turn {
	enemy,	//!< It's yout enemy's turn
	your,	//!< It's your turn
	wait	//!< Waiting ...

};

/**
 * Initialize TicTacToe view
 *
 * Initialize the TicTacToe view with basics elements
 */
extern void view_tictactoe_init();

/**
 * Detach TicTacToe view
 *
 * Detach all elements of the TicTacToe view with basics elements
 */
extern void view_tictactoe_detach();

/**
 * Set case
 *
 * Set a grid case with different elements
 * @param position the position of the case into the grid
 * @param case_type the type of the case
 */
extern void view_tictactoe_set_case(struct utils_position position, enum view_tictactoe_case case_type);


/**
 * Set text
 *
 * Set the text of the tournaments label
 * @param turn turn of player
 */
extern void view_tictactoe_set_turn_text(enum view_tictactoe_turn turn);

/**
 * Set validate button state
 *
 * Set the state of the validate button
 * @param state the state of the button
 */
extern void view_tictactoe_set_validate_button_state(enum utils_button_state state);

#endif
