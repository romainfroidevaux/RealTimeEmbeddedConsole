/*******************************************************************************
 * Console Manager - Source file
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stdbool.h>

#include "consolemanager.h"

#include "serial.h"
#include "view_shell.h"
#include "kernel.h"
#include "model.h"

/*============================================================================*/
/* Local constants and types definitions                                      */
/*============================================================================*/

#define ASCII_BS 8     /**< Backspace char */
#define ASCII_DEL 127  /**< Delete */
#define ASCII_ESC 27   /**< Escape char*/

/*============================================================================*/
/* Global functions                                                           */
/*============================================================================*/

void consolemanager_init() {}

bool consolemanager_get_line(char* line, int size) {
	char c;
	int i;

	for(i = 0; i < size-1; i++) {
		//Wait until a char is received
		while(!serial_tstc()) {
			kernel_reschedule();
		}

		c = serial_getc();

		if (c == '\r') {
			serial_puts("\r\n");
			if (model_get_current_interface() == app_shell)
				view_shell_print_new_line();
			return true;
		} else if(c == '\b') {
			if(i==0) continue;
			if (model_get_current_interface() == app_shell)
				view_shell_delete_char();
			line[i-1] = 0;
			i -= 2;
			serial_puts("\b \b");
			continue;
		} else if (c == ASCII_ESC) {
			continue;
		}

		serial_putc(c);
		if (model_get_current_interface() == app_shell)
			view_shell_print_char(c);

		line[i] = c;
		line[i+1] = 0;
	}
	return false;
}

void consolemanager_clear() {
	serial_puts("\x1b[2J\x1b[1;1H");

	if (model_get_current_interface() == app_shell)
		view_shell_clear();
}

void consolemanager_clearln() {
	serial_puts("\1x1b[2K");

	if (model_get_current_interface() == app_shell)
		view_shell_print_new_line();
}

void consolemanager_print(char* text) {
	serial_puts(text);

	if (model_get_current_interface() == app_shell)
		view_shell_print_chars(text);
}

void consolemanager_println(char* line) {
	serial_puts(line);
	serial_puts("\r\n");

	if (model_get_current_interface() == app_shell) {
		view_shell_print_chars(line);
		view_shell_print_new_line();
	}
}
