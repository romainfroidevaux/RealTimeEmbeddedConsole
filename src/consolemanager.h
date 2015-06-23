/*******************************************************************************
 * Console Manager - Header file
 *
 * This module provide services for the Shell modules.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <stdbool.h>

/**
 * Type for the callback routine when the enter key is pressed.
 *
 * @param line The line
 * @param param Any optional parameter
 */
typedef void (*consolemanager_routine_t)(char* line, void* param);

/**
 * Initialize the Console Manager resources.
 */
extern void consolemanager_init();

/**
 * Reads a whole line from the serial interface.
 *
 * This function give the hand to the next process until a char is received.
 *
 * @param[out] pointer to the line buffer
 * @param[in] buffer maximum size
 * @return The operation status
 * @retval true operation successful (enter key pressed)
 * @retval false the line is to long
 */
extern bool consolemanager_get_line(char* line, int size);

/**
 * Clear the console
 */
extern void consolemanager_clear();

/**
 * Clear the current line.
 */
extern void consolemanager_clearln();

/**
 * Print text on the console.
 *
 * @param text The text to print.
 */
extern void consolemanager_print(char* text);

/**
 * Print a line on the console.
 *
 * @param line The line to print.
 */
extern void consolemanager_println(char* line);

#endif
