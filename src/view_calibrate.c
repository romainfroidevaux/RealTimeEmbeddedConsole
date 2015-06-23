/*******************************************************************************
 * Calibrate View Module - Source file
 *
 * This module provide the view to calibrate the screen
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "view_calibrate.h"
#include "lcddisplay.h"
#include "lcdc.h"
#include "view.h"
#include "touchmanager.h"

struct utils_position old_start_position_z1;
struct utils_position old_stop_position_z1;
struct utils_position old_start_position_z2;
struct utils_position old_stop_position_z2;

void view_calibrate_detach() {
	touchmanager_detach_all();
}

void view_calibrate_init() {

	touchmanager_detach_all();

	//Init all old positions to zero
	old_start_position_z1.x=0;
	old_start_position_z1.y=0;
	old_stop_position_z1.x=0;
	old_stop_position_z1.y=0;
	old_start_position_z2.x=0;
	old_start_position_z2.y=0;
	old_stop_position_z2.x=0;
	old_stop_position_z2.y=0;

	//Reset the screen
	lcddisplay_init();

	//Init structures
	struct utils_position start_position;

	//Print the instruction text
	char* str = "Press the cross to calibrate :-)";
	start_position.x = 65;
	start_position.y = LCD_HEIGHT/2;
	lcddisplay_print_text(str, start_position, black, true);
}

void view_calibrate_set_cross_position(struct utils_position point) {

	//Remove the old cross
	lcddisplay_print_zone(old_start_position_z1, old_stop_position_z1, white);
	lcddisplay_print_zone(old_start_position_z2, old_stop_position_z2, white);

	//Init structures
	struct utils_position start_position;
	struct utils_position stop_position;

	//Print the first zone
	start_position.x = point.x-2;
	start_position.y = point.y-10;
	stop_position.x = point.x+2;
	stop_position.y = point.y+10;
	lcddisplay_print_zone(start_position, stop_position, red);
	old_start_position_z1 = start_position;
	old_stop_position_z1 = stop_position;

	//Print the second zone
	start_position.x = point.x-10;
	start_position.y = point.y-2;
	stop_position.x = point.x+10;
	stop_position.y = point.y+2;
	lcddisplay_print_zone(start_position, stop_position, red);
	old_start_position_z2 = start_position;
	old_stop_position_z2 = stop_position;
}
