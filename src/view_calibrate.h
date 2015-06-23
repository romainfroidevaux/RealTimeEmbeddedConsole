/*******************************************************************************
 * Calibrate View Module - Header file
 *
 * This module provide the view to calibrate the screen
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef VIEW_CLIBRATE_H
#define VIEW_CALIBRATE_H

#include "utils.h"

/**
 * Initialize calibrate view
 *
 * Initialize the calibrate view with basics elements
 */
extern void view_calibrate_init();

/**
 * Detache calibrate view
 *
 * Detach all elements of the calibrate view with basics elements
 */
extern void view_calibrate_detach();

/**
 * Set Cross Position
 *
 * Set the position of the cross calibration
 */
extern void view_calibrate_set_cross_position(struct utils_position);

#endif
