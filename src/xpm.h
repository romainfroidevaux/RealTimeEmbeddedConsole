/*******************************************************************************
 * XPM - XPM File Converter
 *
 * Module to convert xpm files into RGB565 image
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef XPM_H
#define XPM_H
/**
 * EIA-FR - Embedded Systems 1 laboratory
 *
 * Abstract:	XPM - XPM File Converter
 *
 * Purpose:	Module to convert xpm files into RGB565 image
 *
 * Author: 	Daniel Gachet
 * Date: 	12.12.2013
 */

#include <stdint.h>

/**
 * bitmap image
 */
struct xpm_image 
{
	uint32_t width;		// image width in pixels
	uint32_t height;	// image height in pixels
	uint16_t* image;	// image coded in RGB565
};


/**
 * Initialize the xpm module internal resources 
 */
extern void xpm_init();


/**
 * Convert a XPM image into a 16-bit bitmap format RGB565
 * 
 * @param xpm_data xpm-image to be converted
 * @result converted image 
 */
extern struct xpm_image xpm_convert_image (char* xpm_data[]);


#endif
