/*******************************************************************************
 * Shell Back-end Module - Source file
 *
 * This module provide the back-end implementation of the Shell
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "shell_program.h"

#include "utils.h"
#include "chronometer.h"
#include "controller_battleship.h"
#include "controller_home.h"
#include "controller_score.h"
#include "controller_tictactoe.h"
#include "model.h"
#include "ip.h"
#include "shell_interpreter.h"
#include "network.h"
#include "thermometer.h"
#include "consolemanager.h"

#define DEFAULTS_CMD_LEN 12

static char ip_addr_str[15];
static char chrono_str[11];
static struct ip_addr ip_address_arg;


struct ip_addr shell_program_arg_to_ip(int UNUSED(argc), char* argv[]) {

	char* tok;

	//For each argv token, put it on the struct
	//If the token doesn't exist, put -1 on the struct value

	tok = strtok(argv[0], ".");
	if (!tok) ip_address_arg.addrA = -1;
	ip_address_arg.addrA = atoi(tok);

	tok = strtok(NULL, ".");
	if (!tok) ip_address_arg.addrB= -1;
	ip_address_arg.addrB = atoi(tok);

	tok = strtok(NULL, ".");
	if (!tok) ip_address_arg.addrC = -1;
	ip_address_arg.addrC = atoi(tok);

	tok = strtok(NULL, ".");
	if (!tok) ip_address_arg.addrD = -1;
	ip_address_arg.addrD = atoi(tok);

	return ip_address_arg;
}



static char* shell_programm_ip_str(struct ip_addr ip_address) {

	//Create a buffer and initialize it to a null string
	char buf_ip_addr[4];
	char* null_str = "";
	strcpy(ip_addr_str, null_str);

	//Concatenate the struct to XXX.XXX.XXX.XXX
	snprintf(buf_ip_addr, 4, "%d", ip_address.addrA);
	strcat (ip_addr_str, buf_ip_addr);
	strcat (ip_addr_str, ".");
	snprintf(buf_ip_addr, 4, "%d", ip_address.addrB);
	strcat (ip_addr_str, buf_ip_addr);
	strcat (ip_addr_str, ".");
	snprintf(buf_ip_addr, 4, "%d", ip_address.addrC);
	strcat (ip_addr_str, buf_ip_addr);
	strcat (ip_addr_str, ".");
	snprintf(buf_ip_addr, 4, "%d", ip_address.addrD);
	strcat (ip_addr_str, buf_ip_addr);

	return ip_addr_str;
}

void shell_program_ping(int argc, char* argv[]) {

	struct ip_addr ip_address = shell_program_arg_to_ip(argc, argv);

	//Ping the IP address and get the status
	int status = network_ping(ip_address);

	//Print the ping text with IP address
	consolemanager_print("Ping to address ");
	consolemanager_print(shell_programm_ip_str(ip_address));

	//Print the status of the ping
	if (status == 0)
		consolemanager_println(" successfully !");
	else
		consolemanager_println(" failed !");
}
void shell_program_set_ip(int argc, char* argv[]) {
	//int status = -1;

	//Convert the ip address passed by parameter to a struct
	struct ip_addr ip_address = shell_program_arg_to_ip(argc, argv);

	//Set the IP by calling the IP method
	ip_set_ip_address(ip_address);

	//Print texts to informe of the change
	consolemanager_print("IP changes successfully to ");
	consolemanager_println(shell_programm_ip_str(ip_get_ip_address()));

	//status = 0;

	//return status;
}

void shell_program_get_ip(int UNUSED(argc), char* UNUSED(argv[])) {

	//Get the current IP address and put it into a struct
	struct ip_addr ip_address = ip_get_ip_address();

	//Displays the current IP address
	consolemanager_print("The current IP address is : ");
	consolemanager_println(shell_programm_ip_str(ip_address));

	//Return the current IP address (used by the shell_interpreter)
	//return ip_address;
}

void shell_program_app_start(int argc, char* argv[]) {

	//If there is less or more 1 argument, return an error !
	if (argc != 1) return ;

	//int status = -1;

	//Start the application passed on parameter by calling
	//their own start functions
	if (strcasecmp(argv[0], "chronometre") == 0) {
		chronometer_start();
		consolemanager_println("The chronometer app is started");
		//status = 0;

	} else if (strcasecmp(argv[0], "thermometre") == 0) {
		thermometer_thread_start();
		consolemanager_println("The thermometer app is started");
		//status = 0;

	} else if (strcasecmp(argv[0], "battleship") == 0) {
		controller_battleship_start();
		consolemanager_println("The battleship app is started");
		//status = 0;


	} else if (strcasecmp(argv[0], "tictactoe") == 0) {
		controller_tictactoe_start();
		consolemanager_println("The TicTacToe app is started");
		//status = 0;

	} else if (strcasecmp(argv[0], "shell") == 0) {
		controller_home_button_shell_handler(0);
		consolemanager_println("The Shell app is started");
		//status = 0;

	} else if (strcasecmp(argv[0], "home") == 0) {
		controller_home_start();
		consolemanager_println("The Home app is started");
		//status = 0;

	} else if (strcasecmp(argv[0], "scores") == 0) {
		controller_score_start(score_reset);
		consolemanager_println("The Scores app is started");
		//status = 0;

	} else {
		consolemanager_println("Unable to define which app do you want to launch ...");
	}

	//In this case, it would have been more smarter to use a switch case ...
	//I will remember this for the next project :-)

	//return status;
}

void shell_program_app_stop(int argc, char* argv[]) {

	//If there is less or more than 1 parameter, return an error
	if (argc != 1) return;

	//int status = -1;

	//Stop the application passed on parameter by calling
	//their own stop functions
	if (strcasecmp(argv[0], "chronometre") == 0) {
		chronometer_stop();
		consolemanager_println("The chronometer app is stopped");
		//status = 0;

	} else if (strcasecmp(argv[0], "thermometre") == 0) {
		thermometer_thread_stop();
		consolemanager_println("The thermometer app is stopped");
		//status = 0;

	} else if (strcasecmp(argv[0], "battleship") == 0) {
		controller_battleship_stop();
		consolemanager_println("The battleship app is stopped");
		//status = 0;

	} else if (strcasecmp(argv[0], "tictactoe") == 0) {
		controller_tictactoe_stop();
		consolemanager_println("The TicTacToe app is stopped");
		//status = 0;

	} else if (strcasecmp(argv[0], "shell") == 0) {
		controller_home_start();
		consolemanager_println("The Shell app is stopped");
		//status = 0;

	} else if (strcasecmp(argv[0], "home") == 0) {
		controller_home_stop();
		consolemanager_println("The Home app is stopped");
		//status = 0;

	} else if (strcasecmp(argv[0], "scores") == 0) {
		controller_score_stop();
		consolemanager_println("The Scores app is stopped");
		//status = 0;

	} else {
		consolemanager_println("Unable to define which app do you want to stop ...");
	}

	//In this case too, it would have been more smarter to use a switch case ...
	//I will remember this for the next project :-)

	//return status;
}

void shell_program_list_app_all(int UNUSED(argc), char* UNUSED(argv[])) {

	//Display shell text
	consolemanager_println("List of all installed apps :");

	//Get the app list
	char* app_list = model_get_app_list(all);

	//Display it on the shell
	consolemanager_println(app_list);

	//return app_list;
}

void shell_program_list_app_run(int UNUSED(argc), char* UNUSED(argv[])) {

	//Display shell text
	consolemanager_println("List of all running apps :");

	//Get the app list
	char* app_list = model_get_app_list(running);

	//Display it on the shell
	consolemanager_println(app_list);

	//return app_list;
}

void shell_program_list_app_halt(int UNUSED(argc), char* UNUSED(argv[])) {

	//Display shell text
	consolemanager_println("List of all halted apps :");
	//Get the app list
	char* app_list = model_get_app_list(stopped);

	//Display it on the shell
	consolemanager_println(app_list);

	//return app_list;
}

/**char * shell_program_help(int argc, char* argv[]) {
}**/

void shell_program_chrono_start(int UNUSED(argc), char* UNUSED(argv[])) {

	//int status = -1;

	//Start the chronometer. This method start hersels the thread
	// if it is not already started
	chronometer_start();

	//Display shell text
	consolemanager_println("Chronometer started !");

	//status = 0;
	//return status;
}

void shell_program_chrono_stop(int UNUSED(argc), char* UNUSED(argv[]))  {

	//int status = -1;

	//Stop the chronometer and reset it to zero
	chronometer_stop();
	chronometer_reset();

	//Display shell text
	consolemanager_println("Chronometer stopped and reseted to zero !");

	//status = 0;
	//return status;
}

void shell_program_chrono_pause(int UNUSED(argc), char* UNUSED(argv[])) {

	//int status = -1;

	//Stop the chronometer
	chronometer_stop();

	//Display shell text
	consolemanager_println("Chronometer paused ...");

	//status = 0;
	//return status;
}

static char* shell_programm_chrono_str(struct utils_time chrono_value) {

	//Create a buffer and initialize it to a null string
	char buf_chrono[3];
	char* null_str = "";
	strcpy(chrono_str, null_str);

	//Concatenate the struct to HH:MM:SS
	snprintf(buf_chrono, 3, "%d", chrono_value.h);
	strcat (chrono_str, buf_chrono);
	strcat (chrono_str, ":");
	snprintf(buf_chrono, 3, "%d", chrono_value.m);
	strcat (chrono_str, buf_chrono);
	strcat (chrono_str, ":");
	snprintf(buf_chrono, 3, "%d", chrono_value.s);
	strcat (chrono_str, buf_chrono);

	return chrono_str;
}

void shell_program_chrono_show(int UNUSED(argc), char* UNUSED(argv[])) {

	//Get the current chronometer value and put it on a struct
	struct utils_time chrono_value = chronometer_get_time();

	//Display it on the shell text
	consolemanager_print("The current chronometer value is : ");
	consolemanager_println(shell_programm_chrono_str(chrono_value));

	//return chrono_value;
}






//Set an array of avaliable commands to link the function to call and the help
static cmd_t defaults_cmd[] = {
		{
				.name = "ping",
				.cmd_function = shell_program_ping,
				.help = "Ping a remote host.\n\r"
						"Syntax: ping XXX.XXX.XXX.XXX"
		},
		{
				.name = "set_ip",
				.cmd_function = shell_program_set_ip,
				.help = "Set an IP address to the host.\n\r"
						"Syntax: set_ip XXX.XXX.XXX.XXX"
		},
		{
				.name = "get_ip",
				.cmd_function = shell_program_get_ip,
				.help = "Get the current IP address of the host.\n\r"
						"Syntax: get_ip"
		},
		{
				.name = "app_start",
				.cmd_function = shell_program_app_start,
				.help = "Start an application.\n\r"
						"Syntax: app_start APPNAME"
		},
		{
				.name = "app_stop",
				.cmd_function = shell_program_app_stop,
				.help = "Stop an application.\n\r"
						"Syntax: app_stop APPNAME"
		},
		{
				.name = "list_app_all",
				.cmd_function = shell_program_list_app_all,
				.help = "List all apps available in the host.\n\r"
						"Syntax: list_app_all"
		},
		{
				.name = "list_app_run",
				.cmd_function = shell_program_list_app_run,
				.help = "List all running apps in the host.\n\r"
						"Syntax: list_app_run"
		},
		{
				.name = "list_app_halt",
				.cmd_function = shell_program_list_app_halt,
				.help = "List all halted apps in the host.\n\r"
						"Syntax: list_app_halt"
		},
		/** Loic has implemented this function into Shell_interpreter.c
		  {
				.name = "help",
				.cmd_function = shell_program_help,
				.help = "Display help.\n\r"
						"Syntax: help"
		},**/
		{
				.name = "chrono_start",
				.cmd_function = shell_program_chrono_start,
				.help = "Start the chronometer.\n\r"
						"Syntax: chrono_start"
		},
		{
				.name = "chrono_stop",
				.cmd_function = shell_program_chrono_stop,
				.help = "Stop the chronometer.\n\r"
						"Syntax: chrono_stop"
		},
		{
				.name = "chrono_pause",
				.cmd_function = shell_program_chrono_pause,
				.help = "Pause the chronometer.\n\r"
						"Syntax: chrono_pause"
		},
		{
				.name = "chrono_show",
				.cmd_function = shell_program_chrono_show,
				.help = "Display the current value of the chronometer.\n\r"
						"Syntax: chrono_show"
		}
};

void shell_program_init() {
	// Attach all commands to the shell interpreter
	for(int i = 0; i < DEFAULTS_CMD_LEN; i++) {
		shell_interpreter_attach(&defaults_cmd[i]);
	}
}
