/**
 * Serial Interface Drivers - Header file
 *
 * This module provides services for the serial user interface used by the
 * standard input/output functions on UART3.
 *
 * Implementation based on interrupt mode.
 *
 * @author Daniel Gachet <daniel.gachet@hefr.ch>
 * @author Loic Gremaud <loic.gremaud@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 */

#pragma once
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stdbool.h>


/**
 * initialization methods, called by other methods if not previously done.
 *
 * By default the incoming data is enabled.
 */
extern void serial_init();


/**
 * method to change the baud rate
 *
 * @param baudrate interface speed in bit/s
 */
extern void serial_set_baudrate (uint32_t baudrate);


/**
 * method to test if a character is available
 *
 * @return true if a character is available
 */
extern bool serial_tstc();


/**
 * method to read a character from the serial interface.
 * the caller will be suspended until a character is available
 *
 * @return the received character
 */
extern int serial_getc();


/**
 * method to send character on the serial interface
 *
 * @param c character to send over the serial line
 */
extern void serial_putc (char c);


/**
 * method to send a string on the serial interface
 *
 * @param s string to send over the serial line
 */
extern void serial_puts (const char *s);


/**
 * Stop incoming data
 */
extern void serial_stop_rx();

/**
 * Start incoming data
 */
extern void serial_start_rx();


#endif
