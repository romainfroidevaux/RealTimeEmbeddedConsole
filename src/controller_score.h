/*******************************************************************************
 * Controller for Score Module - Header file
 *
 * This module provide services for the Score module.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef CONTROLLER_SCORE_H
#define CONTROLLER_SCORE_H

#include "view_score.h"

/**
 * Replay the last game.
 *
 * @param param : unused param
 */
extern void controller_score_button_replay_handler(void * param);

/**
 * Reset score on this station.
 *
 * @param param : unused param
 */
extern void controller_score_button_reset_handler(void * param);

/**
 * Starts the score app
 *
 */
extern void controller_score_start(enum view_score_button_type type);

/**
 * Stops the score app
 *
 */
extern void controller_score_stop();

#endif
