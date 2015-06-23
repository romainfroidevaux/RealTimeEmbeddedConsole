/*******************************************************************************
 * Controller for the global Application - Header file
 *
 * This module provide services for the controller module.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 * @author Loic Gremaud <loic.gremaud@grelinfo.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#include "model.h"
#include "controller.h"
#include "view_home.h"
#include "controller_home.h"


void controller_button_home_handler(void * UNUSED(param)){
	//consolemanager_println("Debug: controller_button_home_handler");
	model_stop_app(model_get_current_interface());
	model_set_last_game(app_home);

	controller_home_start();
}

