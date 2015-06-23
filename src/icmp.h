/*******************************************************************************
 * Network Module - ICMP header file
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
#ifndef ICMP_H
#define ICMP_H
#include <stdint.h>
// Define some constants.
#define IP4_HDRLEN 20         // IPv4 header length
#define ICMP_HDRLEN 8         // ICMP header length for echo request, excludes data
#define ICMP_ECHO 8
#define ICMP_ECHO_REPLY 0

// ICMP packet structure.
struct icmp {
	//! ICMP message type.
	uint8_t icmp_type;
	//! ICMP operation code.
	uint8_t icmp_code;
	//! ICMP checksum.
	uint16_t icmp_chk;
};

// ICMP::PING packet structure.
typedef struct icmp_ping icmp_hdr_t;
struct icmp_ping {
	// The ICMP header.
	struct icmp ping_icmp;
	// The PING id.
	uint16_t ping_id;
	// The PING sequence number.
	uint16_t ping_seq;
};

extern uint16_t checksum (uint16_t *addr, int len);

#endif /* ICMP_H */
