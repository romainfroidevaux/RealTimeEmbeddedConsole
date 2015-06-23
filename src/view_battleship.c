/*******************************************************************************
 * Battleship Positioning View Module - Source file
 *
 * This module provide the view of positioning phase of the Battleship game
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
#include "view.h"
#include "utils.h"
#include "view_battleship.h"
#include "images/btn_rotate.xpm"
#include "images/btn_play.xpm"
#include "images/img_ship1.xpm"
#include "images/img_ship2.xpm"
#include "images/img_ship3.xpm"
#include "lcddisplay.h"
#include "xpm.h"
#include "controller_battleship.h"
#include "touchmanager.h"

void view_battleship_set_case(struct utils_position position, enum view_battleship_case case_type) {

	if ((position.x > 5) | (position.y > 5)) {
		printf ("ERROR : view_battleship_set_case : position is out of grid\n");
		return;
	}

	struct utils_position string_position;
	struct utils_position start_position;
	struct utils_position stop_position;
	char* string;

	enum view_color case_color;

	//Calculation of the color case zone and text positions
	start_position.x = 321 + (position.x * (3 + 76)) + 3 + 1;
	start_position.y = position.y * (3 + 76) + 1 + 3 + 1;
	stop_position.x = start_position.x + 75;
	stop_position.y = start_position.y + 75;
	string_position.y = start_position.y + 35;

	//Set the color and the text of the case status
	if (case_type == ship_empty) {
		string = "";
		case_color = white;
	} else if (case_type == ship_hit) {
		string = "Hit";
		case_color = red;
		string_position.x = start_position.x + 25;
	} else if (case_type == ship_miss) {
		string = "Miss";
		case_color = blue;
		string_position.x = start_position.x + 22;
	} else if (case_type == ship_select) {
		string = "Select";
		case_color = yellow;
		string_position.x = start_position.x + 13;
	} else if (case_type == ship_present) {
		string = "Ship";
		case_color = green;
		string_position.x = start_position.x + 21;
	} else {
		printf ("ERROR : view_battleship_play_set_case : unknown case_type\n");
		return;
	}

	lcddisplay_print_zone(start_position, stop_position, case_color);

	enum view_color text_color;

	if (case_color == yellow || case_color == green)
		text_color = black;
	else
		text_color = white;

	lcddisplay_print_text(string, string_position, text_color, false);

	printf ("view_battleship_wait_set_case done\n");
}


void view_battleship_init() {

	//Reset the screen
	lcddisplay_init();
	touchmanager_detach_all();

	//Declaration of multi-usage variables
	struct utils_position start_position;
	struct utils_position stop_position;
	struct utils_position string_position;
	struct utils_size size;
	struct xpm_image image;
	char * string;

	//Print default elements common to all views
	view_print_base();

	//Print the separator line between game infos and rotate/play buttons
	start_position.x = 0;
	start_position.y = 348;
	stop_position.x = 320;
	stop_position.y = 350;
	lcddisplay_print_zone(start_position, stop_position, black);

	//Print the grid
	lcddisplay_print_grid(blue, x66);

	start_position.x = 321;
	start_position.y = 1;
	size.h = 478;
	size.w = 478;
	struct utils_size dimension = {6, 6};
	touchmanager_grid_attach(start_position, size, dimension, controller_battleship_grid_handler, 0);

	//Print the first ship
	string = "Submarine";
	string_position.x = 200;
	string_position.y = 145;
	lcddisplay_print_text(string, string_position, black, true);
	image = xpm_convert_image(img_ship1);
	start_position.x = 73;
	start_position.y = 130;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_battleship_button_ship1_handler, 0);

	//Print the second ship
	string = "Destroyer";
	string_position.x = 200;
	string_position.y = 215;
	lcddisplay_print_text(string, string_position, black, true);
	image = xpm_convert_image(img_ship2);
	start_position.x = 57;
	start_position.y = 200;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_battleship_button_ship2_handler, 0);

	//Print the third ship
	string = "Aircraft";
	string_position.x = 200;
	string_position.y = 277;
	lcddisplay_print_text(string, string_position, black, true);
	string = "carrier";
	string_position.x = 200;
	string_position.y = 292;
	lcddisplay_print_text(string, string_position, black, true);
	image = xpm_convert_image(img_ship3);
	start_position.x = 40;
	start_position.y = 270;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_battleship_button_ship3_handler, 0);

	//Print the rotate button
	image = xpm_convert_image(btn_rotate);
	start_position.x = 100;
	start_position.y = 365;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_battleship_button_rotate_handler, 0);

	//Print the play button
	image = xpm_convert_image(btn_play);
	start_position.x = 115;
	start_position.y = 425;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_battleship_play_validate_button_handler, 0);

	printf ("view_battleship_init done\n");
}

void view_battleship_swap() {
	lcddisplay_swap();
}
