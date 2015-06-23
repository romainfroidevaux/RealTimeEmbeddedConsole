/*******************************************************************************
 * Shell Interpreter - Header file
 *
 * This is the application that interpret the text on the console.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/


#pragma once
#ifndef SHELL_INTERPRETER_H
#define SHELL_INTERPRETER_H

#include "utils.h"

/**
 * Command handler function type.
 *
 * @param argc the arguments counter
 * @param argv the arguments in a array of string
 */
typedef void (*cmd_fct_t)(int argc, char *argv[]);

/**
 * Custom command entry type.
 */
typedef struct shell_interpreter_cmd {
	char* name;             ///< Command name.
	cmd_fct_t cmd_function; ///< Command function.
	char* help;             ///< Command help text.
	struct shell_interpreter_cmd* next;            ///< Next for the linked list
} cmd_t;

/**
 * Initialize the Shell Interpreter resource.
 */
extern void shell_interpreter_init();

/**
 * Stop the shell interpreter.
 */
extern void shell_interpreter_start();

/**
 * Start the shell interpreter.
 */
extern void shell_interpreter_stop();

/**
 * Get the state of the shell interpreter application.
 *
 * @return the application state
 */
extern enum utils_app_state shell_interpreter_state();

/**
 * Attach a new command to the Shell interpreter.
 *
 * Do not delete the command type before to call the detach function.
 *
 *
 * @param cmd the pointer to the command type to attach
 */
extern void shell_interpreter_attach(cmd_t *cmd);

/**
 * Detach a command to the Shell interpreter.
 */
extern void shell_interpreter_detach(cmd_t *cmd);

/**
 * Interpret a line.
 *
 * @param line the line to interpret
 */
extern void shell_interpreter_process_line(char* line);

extern char* shell_interpreter_get_current_line();

#endif
