/*******************************************************************************
 * Shell Back-end Module - Header file
 *
 * This module provide the back-end implementation of the Shell
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef SHELL_PROGRAM_H
#define SHELL_PROGRAM_H

/**
 * Initialize Shell back-end
 *
 * Initialize the Shell back-end with basic operations
 */
extern void shell_program_init();

/**
 * Ping
 *
 * Ping an IP address
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return 0 on success, a negative number on failure
 */
extern void shell_program_ping(int argc, char* argv[]);

/**
 * Set IP
 *
 * Set the IP address of the embedded system
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return 0 on success, a negative number on failure
 */
extern void shell_program_set_ip(int argc, char* argv[]);

/**
 * Get IP
 *
 * Get the IP address of the embedded system
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return ip_addr the IP address structure
 */
extern void shell_program_get_ip(int argc, char* argv[]);

/**
 * Start an application
 *
 * Start a process (application) on the embedded system
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return 0 on success, a negative number on failure
 */
extern void shell_program_app_start(int argc, char* argv[]);

/**
 * Stop an application
 *
 * Stop a process (application) on the embedded system
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return 0 on success, a negative number on failure
 */
extern void shell_program_app_stop(int argc, char* argv[]);

/**
 * List all applications
 *
 * List all applications of the embedded system
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return array of chars with all available applications
 */
extern void shell_program_list_app_all(int argc, char* argv[]);

/**
 * List running applications
 *
 * List running applications on the embedded system
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return array of chars with all running applications
 */
extern void shell_program_list_app_run(int argc, char* argv[]);

/**
 * List halted applications
 *
 * List halted applications on the embedded system
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return array of chars with all halted applications
 */
void shell_program_list_app_halt(int argc, char* argv[]);

/**
 * Help
 *
 * Displays all available commands into the shell
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return array of chars with all available commands
 */
extern char * shell_program_help(int argc, char* argv[]);

/**
 * Chrono start
 *
 * Start the chronometer
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return 0 on success, a negative number on failure
 */
extern void shell_program_chrono_start(int argc, char* argv[]);

/**
 * Chrono stop
 *
 * Stop and reset to zero the chronometer
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return 0 on success, a negative number on failure
 */
extern void shell_program_chrono_stop(int argc, char* argv[]);

/**
 * Chrono pause
 *
 * Pause the chronometer
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return 0 on success, a negative number on failure
 */
extern void shell_program_chrono_pause(int argc, char* argv[]);

/**
 * Chrono show
 *
 * Displays into the shell the current value of the chronometer
 * @param argc counter of arguments passed into argv array (argv length)
 * @param argv array of arguments passed on command interpreter
 * @return utils_time the current chronometer value
 */
extern void shell_program_chrono_show(int argc, char* argv[]);

#endif
