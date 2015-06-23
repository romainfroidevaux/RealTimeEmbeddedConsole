/*******************************************************************************
 * TicTacToe View Module - Source file
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

#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
#include "view.h"
#include "view_tictactoe.h"
#include "images/btn_validate.xpm"
#include "images/ttt_circleBlue.xpm"
#include "images/ttt_circleOrange.xpm"
#include "images/ttt_crossBlue.xpm"
#include "images/ttt_crossOrange.xpm"
#include "lcddisplay.h"
#include "xpm.h"
#include "touchmanager.h"
#include "controller_tictactoe.h"

static int tbt_validate = -1;

void view_tictactoe_set_score(uint8_t hit, uint8_t missed) {

	struct utils_position start_position;
	struct utils_position stop_position;
	char* buf_tmp = "";

	//Print the hit value
	snprintf(buf_tmp, 3, "%d", hit);
	start_position.x = 150;
	start_position.y = 240;
	stop_position.x = 172;
	stop_position.y = 248;
	lcddisplay_print_zone(start_position, stop_position, white);
	lcddisplay_print_text(buf_tmp, start_position, black, true);

	//Print the miss value
	snprintf(buf_tmp, 3, "%d", missed);
	start_position.x = 150;
	start_position.y = 260;
	stop_position.x = 172;
	stop_position.y = 268;
	lcddisplay_print_zone(start_position, stop_position, white);
	lcddisplay_print_text(buf_tmp, start_position, black, true);

	printf ("view_tictactoe_set_score done\n");
}

void view_tictactoe_set_validate_button_state(enum utils_button_state state) {

	struct utils_position start_position;
	struct utils_position stop_position;
	struct xpm_image image;


	start_position.x = 90;
	start_position.y = 417;
	stop_position.x = 300;
	stop_position.y = 477;

	if (state == active) {
		image = xpm_convert_image(btn_validate);
		lcddisplay_print_image(start_position, image);
		struct utils_size size;
		size.h = image.height;
		size.w = image.width;
		tbt_validate = touchmanager_button_attach(start_position, size, controller_tictactoe_validate_button_handler, 0);
	} else if (state == inactive) {
		//Not implemented, for future usage ...
		return;
	} else if (state == invisible) {
		lcddisplay_print_zone(start_position, stop_position, white);
		touchmanager_button_detach(tbt_validate);
	}

	printf ("view_tictactoe_set_validate_button_state done\n");
}


void view_tictactoe_init() {

	//Reset the screen
	lcddisplay_init();
	touchmanager_detach_all();

	//Declaration of multi-usage variables
	struct utils_position start_position;
	struct utils_position stop_position;
	struct xpm_image image;
	struct utils_size size;

	view_print_base();

	//Print the separator line between game infos and validate button
	start_position.x = 0;
	start_position.y = 392;
	stop_position.x = 320;
	stop_position.y = 395;
	lcddisplay_print_zone(start_position, stop_position, black);

	//Print the grid
	lcddisplay_print_grid(black, x33);

	start_position.x = 321;
	start_position.y = 1;
	size.h = 478;
	size.w = 478;
	struct utils_size dimension = {3, 3};
	touchmanager_grid_attach(start_position, size, dimension, controller_tictactoe_grid_handler, 0);


	//Print the validate button
	image = xpm_convert_image(btn_validate);
	start_position.x = 90;
	start_position.y = 417;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_tictactoe_validate_button_handler, 0);

	//Init defaults turn
	view_tictactoe_set_turn_text(wait);

	printf ("view_tictactoe_init done\n");
}

void view_tictactoe_set_case(struct utils_position position, enum view_tictactoe_case case_type) {

	if ((position.x > 2) | (position.y > 2)) {
		printf ("ERROR : view_tictactoe_set_case : position is out of grid\n");
		return;
	}

	//Declaration of multiusage variables
	struct utils_position start_position;
	struct utils_position stop_position;

	//Calculation of the color case zone position
	start_position.x = 321 + (position.x * (6 + 152)) + 4;
	start_position.y = position.y * (6 + 152) + 5;
	stop_position.x = start_position.x + 154;
	stop_position.y = start_position.y + 154;

	//Correlate and positioning the image with the case type
	if (case_type == tictactoe_empty)
		lcddisplay_print_zone(start_position, stop_position, white);
	else {
		struct xpm_image image;
		if (case_type == circle_select) {
			start_position.x += 15;
			start_position.y += 15;
			image = xpm_convert_image(ttt_circleOrange);
		} else if (case_type == circle_validate) {
			start_position.x += 15;
			start_position.y += 15;
			image = xpm_convert_image(ttt_circleBlue);
		} else if (case_type == cross_select) {
			start_position.x += 30;
			start_position.y += 30;
			image = xpm_convert_image(ttt_crossOrange);
		} else if (case_type == cross_validate) {
			start_position.x += 30;
			start_position.y += 30;
			image = xpm_convert_image(ttt_crossBlue);
		} else {
			printf ("ERROR : view_tictactoe_set_case : case type unknown\n");
			return;
		}
		lcddisplay_print_image(start_position, image);
	}
}

void view_tictactoe_set_turn_text(enum view_tictactoe_turn turn) {

	char* string = "";

	struct utils_position start_position;
	struct utils_position start_position_zone;
	struct utils_position stop_position_zone;
	start_position.y = 220;

	if (turn == your) {
		string = "It's your turn !";
		start_position.x = 100;
	} else if (turn == enemy) {
		string = "It's your enemy's turn";
		start_position.x = 50;
	} else if (turn == wait) {
		string = "Wait ! Init in progress ...";
		start_position.x = 47;
	} else {
		printf ("ERROR : view_tictactoe_set_turn_text : unknowk turn\n");
		return;
	}

	start_position_zone.x = 30;
	start_position_zone.y = 220;
	stop_position_zone.x = 305;
	stop_position_zone.y = 228;

	lcddisplay_print_zone(start_position_zone, stop_position_zone, white);
	lcddisplay_print_text(string, start_position, black, true);

	printf ("view_tictactoe_set_turn_text done\n");

}
