/*******************************************************************************
 * Network Module - ARP Cache header file
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
#ifndef ARPCACHE_H
#define ARPCACHE_H
#include <stdint.h>
#define ARP_CACHE_LEN 50
/*
 * Define arp_cache struct
 */
typedef struct arp_cache arp_cache_t;
struct arp_cache{
	uint8_t ip[4][ARP_CACHE_LEN];
	uint8_t mac[6][ARP_CACHE_LEN];
	uint32_t cache_size;
};

#endif /* ARPCACHE_H */
