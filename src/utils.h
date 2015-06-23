/*******************************************************************************
 * Utils Module - Header file
 *
 * This module provide useful utilities for others modules
 *
 * @author Romain Froidevaux <romain.froidevaux@edu.hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/**
 * Remove the warning when a function paramter is unused.
 */
#define UNUSED(x) x __attribute__((__unused__))


#define container_of(ptr, type, member) \
	((type*) ((char*)(ptr) - offsetof(type, member)))

/**
 * clamp - return a value clamped to a given range with strict typechecking
 * @val current value
 * @min minimum allowable value
 * @max maximum allowable value
 *
 * This macro does strict typechecking of min/max to make sure they are of the
 * same type as val.  See the unnecessary pointer comparisons.
 */
#define clamp(val, min, max) ((val < min) ? min : (val > max) ? max : val)
 
/**
 * The position
 * A position according to the axes X and Y
 */
struct utils_position {
	int x;	//!< X axis value
	int y;  //!< Y axis value
};

/**
 * A size according to the Height and Width values
 */
struct utils_size {
	uint16_t h;	//!< Height value
	uint16_t w; //!< Width value
};

/**
 * A time set (hours, minutes and seconds)
 */
struct utils_time {
	uint8_t h; //!< Hour value
	uint8_t m; //!< Minutes value
	uint8_t s; //!< Seconds value
};

/**
 * A temperature (XX.YY °C)
 *
 * Number range from -128.99 to 127.99
 */
struct utils_temperature {
	int8_t bdv; //!< Number before dot
	uint8_t adv; //!< Two digit after dot
};

/**
 * States in which a button may be
 */
enum utils_button_state {
	active,		//!< Active and visible
	inactive,	//!< Visible but inactive
	invisible	//!< Invisible and inactive
};

/**
 * Status code
 */
enum utils_status {
	error_indexoutofbounds = -2, ///< Index out of bounds error
	error_nullpointer = -3,      ///< Null pointer error
	error_illegalstate = -4,     ///< Illegal state error
	error_memalloc = -5          ///< Memory allocation error
};

/**
 * State of application
 */
enum utils_app_state{
	all = 3,  		//!< All states
	running = 1,    //!< Running app
	stopped = 2     //!< Stopped app
};


#endif
