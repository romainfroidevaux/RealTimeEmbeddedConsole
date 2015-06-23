/*******************************************************************************
 * Model for the global Application - Header file
 *
 * This module provide services for the Application.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include "model.h"
#include "consolemanager.h"
#include <string.h>

/*
 * TODO: Create a model init with all to stopped
 */
static int appRunning[app_nb] = {
		stopped, stopped, stopped, stopped, stopped, stopped, stopped, stopped, stopped
};
static int lastGame = app_home;
static int currentApp = app_home;
static char str[500];
static char* appText[] = {
	"Battleship",
	"TicTacToe",
	"Shell",
	"Home",
	"Scores",
	"Chronometre",
	"Thermometre",
	"Calibrate",
	"Touchmanager",
	"ShellInterpreter"
};

char* model_get_app_list(enum utils_app_state state){

	strcpy (str, "");

	if((state == running) | (state == all)){
		for (int i = 0; i < app_nb;i++){

			if(appRunning[i] == running){
				strcat (str, appText[i]);
				strcat (str, " is running.\n\r");
			}
		}
	}
	if((state == stopped) | (state == all)){
		for (int i = 0; i < app_nb;i++){
			if(appRunning[i] == stopped){
				strcat (str, appText[i]);
				strcat (str, " is stopped.\n\r");
			}
		}
	}
	return str;
}

void model_start_app(enum model_application app){
	appRunning[app] = running;
	model_set_current_interface(app);
	strcpy(str, appText[app]);
	strcat (str, " is started");
	consolemanager_println(str);
}

int model_get_last_game(){
	return lastGame;
}

void  model_set_last_game(enum model_application app){
	lastGame = app;
}

void model_stop_app(enum model_application app){
	appRunning[app] = stopped;
}

void model_set_current_interface(enum model_application app){
	currentApp = app;
}

int model_get_current_interface(){
	return currentApp;
}
