/*******************************************************************************
 * Network Module - IP header file
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


#ifndef IP_H
#define IP_H
#include <stdint.h>
#include <stdbool.h>
#define IP_V4 4
#define IP_UDP_PROTOCOL 17
#define IP_ICMP_PROTOCOL 1
#define IP_HDRLEN 20

/**
 Defines an IP address A.B.C.D
 */
struct ip_addr {
	uint8_t addrA; //!< A
	uint8_t addrB; //!< B
	uint8_t addrC; //!< C
	uint8_t addrD; //!< D
};

typedef struct ip_hdr ip_hdr_t;
struct ip_hdr{
	uint8_t ip_hdr_len:4; // The header length.
	uint8_t ip_version:4;// The IP version.
	// Type of Service.
	uint8_t ip_tos;
	// IP packet length (both data and header).
	uint16_t ip_len;
	// Identification.
	uint16_t ip_id;
	// Fragment offset.
	uint16_t ip_off;
	// Time To Live.
	uint8_t ip_ttl;
	// The type of the upper-level protocol.
	uint8_t ip_proto;
	// IP header checksum.
	uint16_t ip_chk;
	// IP source address (in network format).
	uint8_t ip_src[4];
	// IP destination address (in network format).
	uint8_t ip_dst[4];
};

/**
 * Compares two IP addresses
 *
 * @param ip1 : first ip address
 * @param ip2 : second ip address
 * @return true if addresses matches
 */
extern bool ip_compareip(struct ip_addr ip1, struct ip_addr ip2);

/**
 * Get the IP address configured
 *
 * @return IP address
 */
extern struct ip_addr ip_get_ip_address();

/**
 * Set an IP address
 *
 * @param : IP address to set
 */
extern void ip_set_ip_address(struct ip_addr ip);

#endif /* IP_H */
