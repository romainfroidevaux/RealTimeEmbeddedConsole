/*******************************************************************************
 * FEC - Fast Ethernet Controller
 *
 * Module to deal with the Fast Ethernet Controller (FEC) of the i.MX27.
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef FEC_H
#define FEC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Method to initialize the Fast Ethernet Controller (FEC)
 *
 *@return status information 0=success, -1=error
 */
extern int fec_init();


/**
 * Start the Fast Ethernet Controller (FEC).
 *
 *@param promiscuous true to run the FEC in promiscuous mode
 *@param broadcast true to allow receiptio of broadcast frames
 *@return status information 0=success, -1=error
 */
extern int fec_start(bool promiscuous, bool broadcast);

/**
 * Stop the Fast Ethernet Controller (FEC).
 *
 *@return status information 0=success, -1=error
 */
extern int fec_stop();


/**
 * Method to receive an Ethernet frame from the FEC
 *
 *@param frame buffer address to store received Ethernet frame
 *@param length buffer size in byte
 *@return size of the received Ethernet frame in byte
 */
extern int fec_recv (void* frame, size_t length);


/**
 * Method to send an Ethernet frame through the FEC
 *
 *@param frame address of the Ethernet frame to be sent
 *@param length frame length in byte
 *@return status information 0=success, -1=error
 */
extern int fec_send (const void* frame, size_t length);


/**
 * Get MAC address from the flash.
 *
 *@param mac Ethernet MAC address
 */
extern void fec_get_mac_address (uint8_t* mac);

/**
 * Set MAC address to the register.
 *
 *@param mac Ethernnet MAC address
 */
extern void fec_set_mac_address (const uint8_t* mac);


#endif
