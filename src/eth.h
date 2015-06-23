/*******************************************************************************
 * Network Module - Ethernet header file
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
#ifndef ETH_H
#define ETH_H

#define ETH_ARP 0x0806
#define ETH_IP4 0x0800
#define ETH_HDRLEN 14

// Define a struct for ethernet header
typedef struct eth_hdr eth_hdr_t;
struct eth_hdr {
	uint8_t target_mac[6]; // 6 bytes
	uint8_t sender_mac[6]; // 6 bytes
	uint16_t protocol; // 2 bytes
};


#endif /* ETH_H */
