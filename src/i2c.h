/*******************************************************************************
 * I2C Device Driver
 *
 * This module provides services for the I2C module of the i.MX27 processor.
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Method to initialize the i2c device driver. 
 * This method should be called prior any other method.
 * 
 */
extern void i2c_init ();


/**
 * Method to set the bus speed.
 * 
 * @param speed bus speed in Hz
 */
extern void i2c_set_bus_speed (uint32_t speed);


/**
 * Method to read data bytes from the internal register files of
 * the specified chip.
 * 
 * @param chip_id chip identification, its address
 * @param reg_addr address of the chip internal register from 
 *                 which the data byte should be read
 * @param buffer data buffer containing the read data bytes
 * @param buffer_len number of data byte to read
 * 
 * @return int status, 0=success, -1=error
 */
extern int i2c_read  (uint8_t chip_id, uint8_t reg_addr, 
                      uint8_t* buffer, uint16_t buffer_len);


/**
 * Method to write data byte into the internal register files of 
 * the specified chip. 
 *  
 * @param chip_id chip identification, its address
 * @param reg_addr address of the chip internal register into 
 *                 which the data byte should be written
 * @param buffer data buffer containing the data bytes to write
 * @param buffer_len number of data byte to write
 * 
 * @return int status, 0=success, -1=error
 */
extern int i2c_write (uint8_t chip_id, uint8_t reg_addr, 
                      const uint8_t* buffer, uint16_t buffer_len);


/**
 * Method to see if a chip is present on the I2C bus.
 * 
 * @param chip_id chip identification, its address
 * 
 * @return bool true if the chip is present, false otherwise
 */
extern bool i2c_probe (uint8_t chip_id);

#endif
