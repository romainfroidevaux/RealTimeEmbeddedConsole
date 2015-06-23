/*******************************************************************************
 * Shell View Module - Header file
 *
 * This module provide the view of the Shell
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef VIEW_SHELL_H
#define VIEW_SHELL_H

#include "utils.h"
#include <stdbool.h>

struct utils_position current_cursor_position;

/**
 * Initialize Shell view
 *
 * Initialize the Shell view with basics elements
 */
extern void view_shell_init();

/**
 * Detach Shell view
 *
 * Detach all elements of the Shell view with basics elements
 */
extern void view_shell_detach();

/**
 * Print chars
 *
 * Print chars into the Shell space
 * @param s string of chars to print
 * > CAN CONTAINS '\N' INTO CHAR TO PRINT A NEW LINE
 * > AUTO NEW LINE IF CHARS ARE TOO LONG
 */
extern void view_shell_print_chars(char* s);

/**
 * Print char
 *
 * Print a unique char into the Shell space
 * @param c char to print
 */
extern void view_shell_print_char(char c);

/**
 * Delete char
 *
 * Delete the last char into the Shell space
 */
extern void view_shell_delete_char();

/**
 * Delete line
 *
 * Delete the current Shell line
 * @param shell_intro print a shell intro for new entry if true
 */
extern void view_shell_delete_line(bool shell_intro);

/**
 * Print new line
 *
 * Create a new line (Carriage return)
 */
extern void view_shell_print_new_line();

/**
 * Clear
 *
 * Clear all printed lines into the Shell
 */
extern void view_shell_clear();

#endif
