/*******************************************************************************
 * Calibrate - Source file
 *
 * Calibration application for the touch screen and API for coordinate
 * conversion.
 *
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef CALIBRATE_H
#define CALIBRATE_H

/**
 * Convert the touchscreen position to the calibrated position.
 *
 * @param p the position to convert
 * @return the calibrated position
 */
extern struct utils_position calibrate_convert(struct utils_position p);

/**
 * Convert the touchscreen position to the calibrated position.
 *
 * @param p the position to convert
 * @return the calibrated position
 */
extern struct utils_position calibrate_convert(struct utils_position p);

/**
 * Start the calibration program.
 */
extern void calibrate_thread_start();

/**
 * Stop the calibration program.
 */
extern void calibrate_thread_stop();

/**
 * Initialize the ressources of the calibrate module.
 */
extern void calibrate_init();

#endif /* CALIBRATE_H */
