#pragma once
#ifndef CSPI_H
#define CSPI_H
/**
 * EIA-FR - Embedded Systems 2 laboratory
 *
 * Abstract:	CSPI - Configurable Serial Peripheral Interface
 *
 * Purpose:	Module to deal with the Configurable Serial Peripheral Interface
 * 		Controller (CSPI) of the i.MX27.
 *
 * Author: 	Daniel Gachet
 * Date: 	12.02.2014
 */

#include <stdint.h>

/**
 * List of available CSPI controllers 
 */
enum cspi_controllers {
	SPI1, 	/**< SPI1 controller */
	SPI2, 	/**< SPI2 controller */
};	

/**
 * List of chip select to be activated during data transfer
 */	
enum cspi_chipselects {
	SS0, 	/**< SS0 will be activated during transfer */
	SS1, 	/**< SS1 will be activated during transfer */
	SS2, 	/**< SS2 will be activated during transfer */
	NO_CS	/**< no chip select will be activated during transfer */
};

/**
 * CSPI Controller 
 */
struct cspi_ctrl {
	enum cspi_controllers ctrl;	/**< CSPI controller */
	enum cspi_chipselects cs;	/**< chip select to be activated */
	
	/**
	 * Method to write a command/data pair over the SPI 
 	 *@param ctrl CSPI controller object
	 *@param command command word
	 *@param data data word
	 */
	void (*write)  (struct cspi_ctrl* ctrl, 
			uint32_t command, 
			uint32_t data);

	/**
	 * Method to read data pair from the SPI 
 	 *@param ctrl CSPI controller object
	 *@param command command word
	 *@param data data words (reference to 1st data word)
	 *@param number number of data words to read
	 */
	void (*read)   (struct cspi_ctrl* ctrl, 
			uint32_t command, 
			uint32_t* data, 
			uint32_t number);

	/* private attributes */
	volatile void* regs;
	void* private_data;
};

/**
 * Method to initialize the resources of a CSPI
 *
 *@param ctrl CSPI controller
 *@param cs chipselect to be activated during transfer
 *@param datarate transmission data rate in kbit/s
 *@param wordsize size of data word in bits
 *@return controller object
 */
extern struct cspi_ctrl cspi_init (enum cspi_controllers ctrl, 
		  		   enum cspi_chipselects cs,
				   uint32_t datarate, 
				   uint32_t wordsize);

#endif

