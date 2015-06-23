/*******************************************************************************
 * Network Module - UDP header file
 *
 * This module provide network services for the controller.
 *
 * @author David Rossier <david.rossier@edu.hefr.ch>
 *
 * @copyright (c) 2014 HES-SO/EIA-FR/TIC/T2a Groupe E
 *
 * @version 1.0.0
 * @date 2014-06-01
 ******************************************************************************/

#pragma once
#ifndef UDP_H
#define UDP_H

struct udp_header{
	uint16_t srcport;
	uint16_t dstport;
	uint16_t length;
	uint16_t checksum;
}udp_header_t;

#endif
