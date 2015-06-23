/*******************************************************************************
 * Network Module - IP C file
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
#include "ip.h"

static struct ip_addr personnal_ip = { 192, 168, 0, 10 };

struct ip_addr ip_get_ip_address() {
	return personnal_ip;
}

void ip_set_ip_address(struct ip_addr ip) {
	personnal_ip = ip;
}
