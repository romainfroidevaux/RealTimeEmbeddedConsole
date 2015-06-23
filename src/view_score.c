/*******************************************************************************
 * Home View Module - Source file
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "view_score.h"
#include "images/btn_replay.xpm"
#include "images/btn_reset.xpm"
#include "images/img_winner.xpm"
#include "images/img_loser.xpm"
#include "images/img_equality.xpm"
#include "lcddisplay.h"
#include "xpm.h"
#include "utils.h"
#include "view.h"
#include "touchmanager.h"
#include "controller_score.h"

/**
 * The different images to illustrate the score
 */
enum view_score_image {
	win,		//!< The win image
	lose,		//!< The lose image
	equality	//!< The equality image
};

void view_score_image_print(enum view_score_image score_image) {
	struct utils_position start_position;
	struct xpm_image image;

	//Set the image and the position in terms of the score
	if (score_image == win) {
		image = xpm_convert_image(img_winner);
		start_position.x = 350;
		start_position.y = 100;
	} else if (score_image == lose) {
		image = xpm_convert_image(img_loser);
		start_position.x = 350;
		start_position.y = 100;
	} else if (score_image == equality) {
		image = xpm_convert_image(img_equality);
		start_position.x = 380;
		start_position.y = 5;
	}else {
		printf ("ERROR : view_score_image_print : score_image unknown\n");
		return;
	}

	lcddisplay_print_image(start_position, image);

	printf ("view_score_image_print done\n");
}

void view_score_text_print() {
	char* string;
	struct utils_position start_position;

	//Print the global scores label
	string = "Global scores";
	start_position.x = 100;
	start_position.y = 170;
	lcddisplay_print_text(string, start_position, black, true);

	//Print the Your score label
	string = "You :";
	start_position.x = 70;
	start_position.y = 240;
	lcddisplay_print_text(string, start_position, black, true);

	//Print the Your enemy's score label
	string = "Enemy :";
	start_position.x = 70;
	start_position.y = 260;
	lcddisplay_print_text(string, start_position, black, true);

	printf ("view_score_text_print done\n");
}

void view_score_values_print(uint8_t player_score, uint8_t enemy_score) {

	struct utils_position start_position;
	char* buf_tmp = "";

	//Print the player's score
	snprintf(buf_tmp, 3, "%d", player_score);
	start_position.x = 150;
	start_position.y = 240;
	lcddisplay_print_text(buf_tmp, start_position, black, true);

	//Print the enemy's score
	snprintf(buf_tmp, 3, "%d", enemy_score);
	start_position.x = 150;
	start_position.y = 260;
	lcddisplay_print_text(buf_tmp, start_position, black, true);

	printf ("view_score_values_print done\n");
}

void view_score_button_print(enum view_score_button_type button_type) {

	struct utils_position start_position;
	struct xpm_image image;
	struct utils_size size;

	touchmanager_button_routine_t callback = 0;

	//Print the replay or reset button
	if (button_type == score_replay) {
		image = xpm_convert_image(btn_replay);
		size.h = image.height;
		size.w = image.width;

		callback = controller_score_button_replay_handler;

	} else if (button_type == score_reset) {
		image = xpm_convert_image(btn_reset);
		callback = controller_score_button_reset_handler;
	} else {
		printf ("ERROR : view_score_init : button type unknown\n");
		return;
	}

	start_position.x = 90;
	start_position.y = 417;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;

	touchmanager_button_attach(start_position, size, callback, 0);

	printf ("view_score_button_print done\n");
}

void view_score_detach() {
	touchmanager_detach_all();
}

void view_score_init(uint8_t player_score, uint8_t enemy_score, enum view_score_button_type button_type) {

	//Reset the screen
	lcddisplay_init();
	touchmanager_detach_all();

	//Declaration of multi-usage variables
	struct utils_position start_position;
	struct utils_position stop_position;

	//Print default elements common to all views
	view_print_base();

	//Print the separator line between scores and validate button
	start_position.x = 0;
	start_position.y = 392;
	stop_position.x = 320;
	stop_position.y = 395;
	lcddisplay_print_zone(start_position, stop_position, black);

	view_score_button_print(button_type);
	view_score_text_print();
	view_score_values_print(player_score, enemy_score);


	if (player_score > enemy_score)
		view_score_image_print(win);
	else if (player_score < enemy_score)
		view_score_image_print(lose);
	else
		view_score_image_print(equality);

	printf ("view_score_init done\n");
}
