/*******************************************************************************
 * Battleship Wait View Module - Source file
 *
 * This module provide the view of waiting phase of the Battleship game
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
#include "view_battleship_wait.h"
#include "view_battleship.h"
#include "utils.h"
#include "xpm.h"
#include "lcddisplay.h"
#include "view.h"
#include "touchmanager.h"

void view_battleship_wait_set_score(uint8_t hit, uint8_t missed) {

	struct utils_position start_position;
	struct utils_position stop_position;
	char* buf_tmp = "";

	//Print the hit value
	snprintf(buf_tmp, 3, "%d", hit);
	start_position.x = 150;
	start_position.y = 350;
	stop_position.x = 172;
	stop_position.y = 358;
	lcddisplay_print_zone(start_position, stop_position, white);
	lcddisplay_print_text(buf_tmp, start_position, black, true);

	//Print the miss value
	snprintf(buf_tmp, 3, "%d", missed);
	start_position.x = 150;
	start_position.y = 370;
	stop_position.x = 172;
	stop_position.y = 378;
	lcddisplay_print_zone(start_position, stop_position, white);
	lcddisplay_print_text(buf_tmp, start_position, black, true);

	printf ("view_battleship_wait_set_score done\n");
}

void view_battleship_wait_init() {

	//Reset the screen
	lcddisplay_init();
	touchmanager_detach_all();

	//Declaration of multi-usage variables
	struct utils_position start_position;

	//Print default elements common to all views
	view_print_base();

	//Print the grid
	lcddisplay_print_grid(blue, x66);

	//Print turning text
	char* str_turn = "It's your enemy's turn !";
	start_position.x = 70;
	start_position.y = 230;
	lcddisplay_print_text(str_turn, start_position, black, true);

	//Print nb_hit text
	char* str_nb_hit = "Nb hits :";
	start_position.x = 60;
	start_position.y = 350;
	lcddisplay_print_text(str_nb_hit, start_position, black, true);

	//Print nb_miss_text
	char* str_nb_miss = "Nb miss :";
	start_position.x = 60;
	start_position.y = 370;
	lcddisplay_print_text(str_nb_miss, start_position, black, true);


	//Init defaults scores
	view_battleship_wait_set_score(0,0);

	printf ("view_battleship_wait_init done\n");
}

void view_battleship_wait_detach() {
	touchmanager_detach_all();
}
