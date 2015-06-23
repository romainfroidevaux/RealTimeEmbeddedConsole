/*******************************************************************************
 * Shell View Module - Source file
 *
 * This module provide the view of the Shell
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

//#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "view.h"
#include "view_shell.h"
#include "xpm.h"
#include "lcddisplay.h"
#include "images/img_shell.xpm"
#include "touchmanager.h"
#include "shell_interpreter.h"
#include "model.h"

void view_shell_detach() {
	touchmanager_detach_all();
}

void view_shell_init() {
	//Reset the screen
	lcddisplay_init();
	touchmanager_detach_all();

	//Declaration of multi-usage variables
	struct utils_position start_position;
	struct xpm_image image;

	//Print default elements common to all views
	view_print_base();

	//Print the shell image
	image = xpm_convert_image(img_shell);
	start_position.x = 10;
	start_position.y = 150;
	lcddisplay_print_image(start_position, image);

	view_shell_clear();

	//printf ("view_shell_init done\n");
}

void view_shell_reset_shell() {
	//Print the black zone of the shell
	struct utils_position start_position;
	struct utils_position stop_position;
	start_position.x = 325;
	start_position.y = 5;
	stop_position.x = 795;
	stop_position.y = 475;
	lcddisplay_print_zone(start_position, stop_position, black);

	//printf ("view_shell_reset_shell done\n");
}

void view_shell_init_cursor() {
	//Init the cursor at the top left position of the shell
	current_cursor_position.x = 330;
	current_cursor_position.y = 10;

	//printf ("view_shell_init_cursor done\n");
}

void view_shell_print_default_intro() {
	char* string = "sh-groupe_e # ";
	view_shell_print_chars(string);

	//printf ("view_shell_print_default_intro done\n");
}

void view_shell_clear () {
	view_shell_reset_shell();
	view_shell_init_cursor();
	//view_shell_print_default_intro();

	view_shell_print_chars("$ ");
	view_shell_print_chars(shell_interpreter_get_current_line());

	//printf ("view_shell_clear done\n");
}

bool view_shell_print_chars_tests (char sel_char) {
	bool status = false;

	if (current_cursor_position.y > 465) {
		view_shell_reset_shell();
		view_shell_init_cursor();
	} else if (current_cursor_position.x > 782) {
		view_shell_print_new_line();
	} else if (sel_char == '\n') {
		view_shell_print_new_line();
		status =  true;
	} else if (sel_char == '\r') {
		status = true;
	}

	//printf ("view_shell_print_chars_tests done\n");

	return status;

}

void view_shell_print_chars(char* s) {

		char sel_char;

			do {
				sel_char = *s++;

				if (view_shell_print_chars_tests(sel_char))
					continue;

				view_shell_print_char(sel_char);

			} while (sel_char != '\0');

}

void view_shell_print_char(char c) {
	if(model_get_current_interface() == app_shell){
		view_shell_print_chars_tests(c);

		lcddisplay_print_char(c, current_cursor_position, white, false);

		current_cursor_position.x += 8;
	}
	//printf ("view_shell_print_char done\n");
}

void view_shell_print_new_line() {
	current_cursor_position.x = 330;
	current_cursor_position.y += 10;

	if (current_cursor_position.y > 465) {
		view_shell_reset_shell();
		view_shell_init_cursor();
	}
}

void view_shell_delete_char() {
	if ((current_cursor_position.x == 330) || (current_cursor_position.x == 442)) {
		printf ("ERROR : view_shell_delete_char : no char is deletable (line empty)\n");
		return;
	}

	current_cursor_position.x -= 8;
	struct utils_position stop_position;
	stop_position.x = current_cursor_position.x + 8;
	stop_position.y = current_cursor_position.y + 8;
	lcddisplay_print_zone(current_cursor_position, stop_position, black);

	//printf ("view_shell_delete_char done\n");
}

void view_shell_delete_line(bool shell_intro) {
	struct utils_position stop_position;
	current_cursor_position.x = 330;
	stop_position.x = 795;
	stop_position.y = current_cursor_position.y + 8;
	lcddisplay_print_zone(current_cursor_position, stop_position, black);

	if (shell_intro)
		view_shell_print_default_intro();

	//printf ("view_shell_delete_line done\n");
}
