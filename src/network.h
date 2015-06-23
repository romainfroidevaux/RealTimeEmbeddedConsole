/*******************************************************************************
 * Network Module - Header file
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
#ifndef NETWORK_H
#define NETWORK_H

#include "ip.h"
#include "icmp.h"
#include "arpcache.h"

arp_cache_t arpcache;

/* private */
/**
 * Send ICMP reply
 * @param target_ip
 * @param target_mac
 * @param icmp
 * @param ip
 * @param data
 */
void network_send_icmp_reply(uint8_t *target_ip, uint8_t *target_mac, icmp_hdr_t *icmp, ip_hdr_t *ip, uint8_t *data);

/**
 * Send ARP reply
 * @param target_ip
 * @param target_mac
 */
void network_send_arp_reply(uint8_t *target_ip, uint8_t *target_mac);
/* public */

/**
 * Inits network
 */
extern void network_init();

/**
 * Receive 1 frame.
 */
extern void network_receive_frame();

/**
 * Send ICMP request (ping)
 * @param ip_dest
 * @return
 */
extern int network_ping(struct ip_addr ip_dest);

/**
 * Send arp request
 * @param dest_ip_0
 * @param dest_ip_1
 * @param dest_ip_2
 * @param dest_ip_3
 */
extern void network_send_arp_request(uint8_t dest_ip_0,uint8_t dest_ip_1,uint8_t dest_ip_2,uint8_t dest_ip_3);

/**
 * Thread managing receiving frames.
 */
extern void network_receive();

/**
 * Stops the network
 */
extern void close_network();

#endif /* NETWORK_H */
