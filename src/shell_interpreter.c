/*******************************************************************************
 * Shell Interpreter - Source file
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

//A ne pas reproduire
//Temporaire
#undef __STRICT_ANSI__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "shell_interpreter.h"

#include "utils.h"
#include "serial.h"
#include "kernel.h"
#include "consolemanager.h"
#include "model.h"

/*============================================================================*/
/* Local constants and types definitions                                      */
/*============================================================================*/

#define MAX_ARGUMENTS 10    /**< The maximum number of arguments. */
#define MAX_LINE_LEN 80+1     /**< The maximum number of char for a line. */
#define MAX_COMMANDS 100     /**< The maximum number of commands */

/*============================================================================*/
/* Local variables                                                            */
/*============================================================================*/


static bool run = false;

/**
 * The line buffer.
 */
static char line_buffer[MAX_LINE_LEN] = "";

static char* prompt = "$ ";


static cmd_t* head = 0;
static int count = 0;

/*============================================================================*/
/* Local functions                                                            */
/*============================================================================*/


static void cmd_help(int argc, char *argv[]) {
	//Precondition
	if(argc != 1) return;

	for(cmd_t* c = head; c != 0; c = c->next) {
		if(strcasecmp(argv[0], c->name) == 0) {
			consolemanager_println(c->help);
			return;
		}
	}
	consolemanager_println("help: command not exist");
}

static void cmd_test(int argc, char *argv[]) {

	if (argc == 0) {
		consolemanager_println("No argument");
		return;
	}

	consolemanager_println("Arguments list:");

	for(int i = 0; i < argc; i++) {
		consolemanager_println(argv[i]);
	}
}

static void cmd_list(int argc, char* UNUSED(argv[])) {
	//Precondition
	if(argc != 0) {
		consolemanager_println("no argument required");
	}

	consolemanager_println("Command list:");
	for(cmd_t* c = head; c != 0; c = c->next) {
		consolemanager_print(" - ");
		consolemanager_println(c->name);
	}
}

static void cmd_clear(int argc,char *UNUSED(argv[])) {
	//Precondition
	if(argc != 0) {
		consolemanager_println("no argument required");
	}
	consolemanager_clear();
}

static inline void clear_line_buffer() {
	line_buffer[0] = 0;
}



static void thread (void* UNUSED(param)) {
	run = true;

	while (run) {
		consolemanager_print(prompt);
		if (consolemanager_get_line(line_buffer, MAX_LINE_LEN)) {
			shell_interpreter_process_line(line_buffer);
			clear_line_buffer();
		}
		kernel_reschedule();
	}
	kernel_thread_exit();
}

/*============================================================================*/
/* Local variables - Part 2                                                   */
/*============================================================================*/

static cmd_t defaults_cmd[] = {
		{
				.name = "help",
				.cmd_function = cmd_help,
				.help = "Displays help for a command.\n\r"
						"Syntax: help [command name]"
		},
		{
				.name = "test",
				.cmd_function = cmd_test,
				.help = "Test program displays all parameters.\n\r"
						"Syntax: test [parameter...]"
		},
		{
				.name = "clear",
				.cmd_function = cmd_clear,
				.help = "Clear the console.\n\r"
						"Syntax: clear"
		},
		{
				.name = "list",
				.cmd_function = cmd_list,
				.help = "List all command name."
		},
		{
				.name = 0 //Signal the end of array
		}
};

/*============================================================================*/
/* External functions                                                         */
/*============================================================================*/

//char* shell_interpreter_prompt() {
//	return &prompt;
//}

void shell_interpreter_attach(cmd_t *cmd) {
	//Precondition
	if (cmd == 0 || count >= MAX_COMMANDS) return;

	//Precondition: cmd is not already in list
	for(cmd_t* c = head; c != NULL; c = c->next) {
		if(c == cmd) return;
	}

	// Add to the head
	cmd->next = head;
	head = cmd;
	count++;
}

void shell_interpreter_init() {
	// Attach all defaults commands
	for(int i = 0; defaults_cmd[i].name != 0 ; i++) {
		shell_interpreter_attach(&defaults_cmd[i]);
	}
}

void shell_interpreter_start() {
	//Precondition
	if (run) return;

	kernel_thread_create(thread, 0);
	model_start_app(app_shellinterpreter);
}

void shell_interpreter_stop() {
	run = false;
	model_stop_app(app_shellinterpreter);
}

void shell_interpreter_detach(cmd_t *cmd) {
	//Precondition
	if(cmd == 0 || count == 0) return;

	cmd_t* cur = head;
	cmd_t* prev = 0;

	while(cur != 0) {
		if(cur == cmd) {
			if(prev == 0) {
				//assert(first == cmd)
				head = cmd->next;
			} else {
				//assert(prev.next == cmd)
				prev->next = cmd->next;
			}
			count--;
			return;
		}
		prev = cur;
		cur = cur->next;
	}
}

char* shell_interpreter_get_current_line() {
	return line_buffer;
}

void shell_interpreter_process_line(char* line) {
	char *lp, *saveptr, *cmd;
	char *argv[MAX_ARGUMENTS];
	int argc;

	// Parse the line_buffer
	lp = (char*) strtok_r(line, " \t", &saveptr);
	cmd = lp;
	argc = 0;
	while ((lp = (char*) strtok_r(0, " \t", &saveptr)) != 0) {
		if (argc >= MAX_ARGUMENTS) {
			consolemanager_println("too many arguments");
			cmd = 0;
			break;
		}
		argv[argc++] = lp;
	}
	// End parse the line

	//Find the command and execute it
	if(strcasecmp(cmd, "") == 0) {;
		consolemanager_println("");
		return;
	}

	for(cmd_t* c = head; c != NULL; c = c->next) {
		if(strcasecmp(cmd, c->name) == 0) {
			c->cmd_function(argc, argv);
			return;
		}
	}

	//Skip error on blank line
	if (line[0] != 0) {
		consolemanager_println("command not exist");
	}
}
