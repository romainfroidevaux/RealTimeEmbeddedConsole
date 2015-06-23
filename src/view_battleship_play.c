/*******************************************************************************
 * Battleship Play View Module - Source file
 *
 * This module provide the view of play phase of the Battleship game
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "view_battleship_play.h"
#include "view_battleship.h"
#include "utils.h"
#include "images/btn_validate.xpm"
#include "lcddisplay.h"
#include "xpm.h"
#include "view.h"
#include "controller_battleship.h"
#include "touchmanager.h"

int btn_validate_int = -1;

void view_battleship_play_set_score(uint8_t hit, uint8_t missed) {

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

	printf ("view_battleship_play_set_score done\n");
}

void view_battleship_play_init() {

	//Reset the screen
	lcddisplay_init();
	touchmanager_detach_all();

	//Declaration of multi-usage variables
	struct utils_position start_position;
	struct utils_position stop_position;
	struct xpm_image image;
	struct utils_size size;

	//Print default elements common to all views
	view_print_base();

	//Print the separator line between game infos and validate button
	start_position.x = 0;
	start_position.y = 392;
	stop_position.x = 320;
	stop_position.y = 395;
	lcddisplay_print_zone(start_position, stop_position, black);

	//Print the grid
	lcddisplay_print_grid(red, x66);

	//Print turning text
	char* str_turn = "It's your turn !";
	start_position.x = 90;
	start_position.y = 180;
	lcddisplay_print_text(str_turn, start_position, black, true);

	//Print nb_hit text
	char* str_nb_hit = "Nb hits :";
	start_position.x = 60;
	start_position.y = 240;
	lcddisplay_print_text(str_nb_hit, start_position, black, true);

	//Print nb_miss_text
	char* str_nb_miss = "Nb miss :";
	start_position.x = 60;
	start_position.y = 260;
	lcddisplay_print_text(str_nb_miss, start_position, black, true);

	//Print the validate button
	image = xpm_convert_image(btn_validate);
	start_position.x = 90;
	start_position.y = 417;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	btn_validate_int = touchmanager_button_attach(start_position, size, controller_battleship_play_validate_button_handler, 0);

	//Init defaults scores
	view_battleship_play_set_score(0,0);

	printf ("view_battleship_play_init done\n");
}

void view_battleship_play_set_validate_button_state(enum utils_button_state state) {

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
		btn_validate_int = touchmanager_button_attach(start_position, size, controller_battleship_play_validate_button_handler, 0);

	} else if (state == inactive) {
		//Not implemented, for future usage ...
		//We could for example print a grey button ?
		return;
	} else if (state == invisible) {
		lcddisplay_print_zone(start_position, stop_position, white);
		touchmanager_button_detach(btn_validate_int);
	}

	printf ("view_battleship_play_set_validate_button_state done\n");
}

void view_battleship_play_detach() {
	touchmanager_detach_all();
}
