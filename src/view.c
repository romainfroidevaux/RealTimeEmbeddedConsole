/*******************************************************************************
 * View Module - Source file
 *
 * This module provide the main view controller
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include "utils.h"
#include "view.h"
#include "xpm.h"
#include "lcddisplay.h"
#include "images/btn_returnToHome.xpm"
#include "controller.h"
#include "touchmanager.h"

void view_init() {
	return;
}

void view_detach() {
	touchmanager_detach_all();
}

void view_swap() {
	lcddisplay_swap();
}

void view_print_base() {

	//Declaration of multi-usage variables
	struct utils_position start_position;
	struct utils_position stop_position;
	struct xpm_image image;
	struct utils_size size;

	//Print the returnToHome button
	image = xpm_convert_image(btn_returnToHome);
	start_position.x = 40;
	start_position.y = 25;
	lcddisplay_print_image(start_position, image);
	size.h = image.height;
	size.w = image.width;
	touchmanager_button_attach(start_position, size, controller_button_home_handler, 0);

	//Print the separator line between grid and button area
	start_position.x = 317;
	start_position.y = 0;
	stop_position.x = 320;
	stop_position.y = 480;
	lcddisplay_print_zone(start_position, stop_position, black);

	//Print the separator line between home button and game infos
	start_position.x = 0;
	start_position.y = 95;
	stop_position.x = 320;
	stop_position.y = 98;
	lcddisplay_print_zone(start_position, stop_position, black);
}
