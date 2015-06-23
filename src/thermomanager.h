/*******************************************************************************
 * Thermometer Manager - Header file
 *
 * This module provide services for the Thermometer Application.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
  ******************************************************************************/

#pragma once
#ifndef THERMOMANAGER_H
#define THERMOMANAGER_H

#include "utils.h"

/**
 * Initialize the Thermometar Manager ressources.
 */
extern void thermometer_init();

/**
 * Get the current temperature.
 *
 * @param temp The retrieved temperature
 * @return 0 on success, a negative value on failure
 */
extern int thermometer_get_temp(struct utils_int8_dot_1dig* temp);
