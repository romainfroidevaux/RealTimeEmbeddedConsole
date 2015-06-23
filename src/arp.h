/*******************************************************************************
 * Network Module - ARP header file
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


#ifndef ARP_H
#define ARP_H
#include <stdint.h>
#define ARP_HW_TYPE 1 // hardware type : ethernet
#define ARP_PROT_TYPE 0x0800 // protocol type : IPv4
#define ARP_HW_ADD_LEN 6 // hardware address length : ethernet
#define ARP_PROT_ADD_LEN 4 // protocol address length : IPv4
#define ARP_REQUEST 1
#define ARP_REPLY 2
#define ARP_HDRLEN 28
/*
 * Define a struct for ARP header
 */
typedef struct arp_hdr arp_hdr_t;
struct arp_hdr {
	uint16_t htype;
	uint16_t ptype;
	uint8_t hlen;
	uint8_t plen;
	uint16_t opcode;
	uint8_t sender_mac[6];
	uint8_t sender_ip[4];
	uint8_t target_mac[6];
	uint8_t target_ip[4];
};

#endif /* ARP_H */
