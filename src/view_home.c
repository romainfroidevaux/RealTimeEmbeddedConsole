/*******************************************************************************
 * Home View Module - Source file
 *
 * This module provide the view for the home screen
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch>
 * @author Loic Gremaud <loic.gremaud@grelinfo.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"
#include "view.h"
#include "lcddisplay.h"
#include "xpm.h"
#include "images/logo_eia.xpm"
#include "images/img_stopwatch.xpm"
#include "images/img_thermometer.xpm"
#include "images/btn_battleship.xpm"
#include "images/btn_ticTacToe.xpm"
#include "images/btn_shell.xpm"
#include "images/btn_scores.xpm"
#include "view_home.h"
#include "touchmanager.h"
#include "controller_home.h"
#include "model.h"

void view_home_set_temperature(struct utils_temperature temperature) {

	//Set temperature value only if we are on the home screen
	if (model_get_current_interface() == app_home) {

		//Creation of the positions and the string buffer
		struct utils_position start_position;
		struct utils_position stop_position;
		char buf_tmp[5];
		char str[10];

		//Convert the temp struct to str
		snprintf(buf_tmp, 5, "%d", temperature.bdv);
		strcpy (str, buf_tmp);
		strcat (str, ".");
		snprintf(buf_tmp, 5, "%d", temperature.adv);
		strcat (str, buf_tmp);
		strcat (str, "'C");

		start_position.x = 190;
		start_position.y = 385;
		stop_position.x = 262;
		stop_position.y = 393;

		//Delete the zone because in case of smaller temp, the old biggest symbol is not removed
		lcddisplay_print_zone(start_position, stop_position, white);

		lcddisplay_print_text(str, start_position, black, true);

		printf ("view_home_set_temperature done\n");
	}
}

void view_home_set_time(struct utils_time time) {

	//Set temperature value only if we are on the home screen
	if (model_get_current_interface() == app_home) {

		if ((time.h > 99) | (time.m > 99) | (time.s > 99)) {
			printf ("ERROR : view_home_set_time : hour, minute or second is greater than 99\n");
			return;
		}

		struct utils_position start_position;
		char buf_tmp[3];
		char str[10];

		//Convert the time struct to str
		//Put a 0 before the number if it's less than 10
		snprintf(buf_tmp, 3, "%d", time.h);

		if (time.h < 10) {
			strcpy (str, "0");
			strcat (str, buf_tmp);
		} else
			strcpy (str, buf_tmp);

		strcat (str, ":");

		if (time.m < 10)
			strcat (str,"0");

		snprintf(buf_tmp, 3, "%d", time.m);
		strcat (str, buf_tmp);
		strcat (str, ":");

		snprintf(buf_tmp, 3, "%d", time.s);
		if (time.s < 10)
			strcat (str,"0");

		strcat (str, buf_tmp);

		start_position.x = 190;
		start_position.y = 210;

		lcddisplay_print_text(str, start_position, black, true);

		printf ("view_home_set_time done\n");
	}
}

void view_home_init() {

	//Reset the screen
	lcddisplay_init();
	touchmanager_detach_all();

	//Declaration of multi-usage positions structures and image structure
	struct utils_position start_position;
	struct utils_position stop_position;
	struct utils_size size;
	struct xpm_image image;

	//Print the logo on top left corner
	image = xpm_convert_image(logo_eia);
	start_position.x = 35;
	start_position.y = 20;
	lcddisplay_print_image(start_position, image);

	//Print separator zone under the logo
	start_position.x = 0;
	start_position.y = 120;
	stop_position.x = 320;
	stop_position.y = 123;
	lcddisplay_print_zone(start_position, stop_position, black);

	//Print separator zone on right of the logo
	start_position.x = 320;
	start_position.y = 0;
	stop_position.x = 323;
	stop_position.y = 480;
	lcddisplay_print_zone(start_position, stop_position, black);

	//Print separator zone between stopwatch and thermometer
	start_position.x = 0;
	start_position.y = 298;
	stop_position.x = 320;
	stop_position.y = 301;
	lcddisplay_print_zone(start_position, stop_position, black);

	//Print stopwatch image
	image = xpm_convert_image(img_stopwatch);
	start_position.x = 20;
	start_position.y = 140;
	lcddisplay_print_image(start_position, image);

	//Print thermometer image
	image = xpm_convert_image(img_thermometer);
	start_position.x = 55;
	start_position.y = 325;
	lcddisplay_print_image(start_position, image);

	//Print zero values to thermometer and stopwatch
	struct utils_temperature temp;
	temp.bdv = 0;
	temp.adv = 0;
	struct utils_time time;
	time.h = 0;
	time.m = 0;
	time.s = 0;
	view_home_set_temperature(temp);
	view_home_set_time(time);

	//Print Battleship App button
	image = xpm_convert_image(btn_battleship);
	start_position.x = 360;
	start_position.y = 50;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_home_button_battleship_handler, 0);

	//Print TicTacToe App button
	image = xpm_convert_image(btn_ticTacToe);
	start_position.x = 570;
	start_position.y = 50;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_home_button_tictactoe_handler, 0);

	//Print Shell App button
	image = xpm_convert_image(btn_shell);
	start_position.x = 360;
	start_position.y = 185;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_home_button_shell_handler, 0);

	//Print Scores App button
	image = xpm_convert_image(btn_scores);
	start_position.x = 570;
	start_position.y = 185;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_home_button_score_handler, 0);

	//Print Copyright
	char* string1 = "Software developed by :";
	char* string2 = "L. Gremaud, D. Rossier and R. Froidevaux";
	start_position.x = 400;
	start_position.y = 385;
	lcddisplay_print_text(string1, start_position, black, true);
	start_position.y = 395;
	lcddisplay_print_text(string2, start_position, black, true);

	printf ("view_home_init done\n");
}
