/*******************************************************************************
 * Network Module - Network file
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

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"
#include "eth.h"
#include "fec.h"
#include "arp.h"
#include "kernel.h"
#include "ip.h"
#include "icmp.h"
#include "network.h"
#include "arpcache.h"
#include "udp.h"
#include "utils.h"
#include "consolemanager.h"

#define IP_MAXPACKET 1500

/**
 * Macros to convert host to network addresses
 */
#define htons(x) (((((uint16_t)(x)) >> 8) & 0xff) | \
                  ((((uint16_t)(x)) & 0xff) << 8))

#define ntohs(x) (((((uint16_t)(x)) >> 8) & 0xff) | \
                  ((((uint16_t)(x)) & 0xff) << 8) )

#define htonl(x) ((((x)>>24) & 0xffl)   | (((x)>>8)  & 0xff00l) | \
                 (((x)<<8) & 0xff0000l) | (((x)<<24) & 0xff000000l))

#define ntohl(x) ((((x)>>24) & 0xffl)   | (((x)>>8)  & 0xff00l) | \
                 (((x)<<8) & 0xff0000l) | (((x)<<24) & 0xff000000l))

void network_stop() {
	fec_stop();
}

void network_init_fec() {
	fec_init();
	fec_start(false, true);
}

void network_init() {
	arpcache.cache_size = 0;
	network_init_fec();
	kernel_thread_create(network_receive, 0);
	struct ip_addr default_adr = { 0xc0, 0xa8, 0x00, 0x0a };
	ip_set_ip_address(default_adr);
}

void network_receive(void* UNUSED(param)) {
	while (1) {
		network_receive_frame();
		kernel_reschedule();
	}
	kernel_thread_exit();
}

// Checksum function
uint16_t checksum(uint16_t *addr, int len) {
	int nleft = len;
	int sum = 0;
	uint16_t *w = addr;
	uint16_t answer = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= sizeof(uint16_t);
	}

	if (nleft == 1) {
		*(uint8_t *) (&answer) = *(uint8_t *) w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}

void network_send_arp_request(uint8_t dest_ip_0, uint8_t dest_ip_1,
		uint8_t dest_ip_2, uint8_t dest_ip_3) {

	//consolemanager_println("Sending ARP request...");

	/* init structs size */

	arp_hdr_t arp;
	eth_hdr_t eth;
	uint8_t *eth_frame;
	eth_frame = (uint8_t *) malloc(IP_MAXPACKET * sizeof(uint8_t));
	memset(eth_frame, 0, IP_MAXPACKET);

	/* arp frame */

	struct ip_addr myip = ip_get_ip_address();
	arp.sender_ip[0] = myip.addrA;
	arp.sender_ip[1] = myip.addrB;
	arp.sender_ip[2] = myip.addrC;
	arp.sender_ip[3] = myip.addrD;
	arp.target_ip[0] = dest_ip_0;
	arp.target_ip[1] = dest_ip_1;
	arp.target_ip[2] = dest_ip_2;
	arp.target_ip[3] = dest_ip_3;
	memset(arp.target_mac, 0xff, 6); // Set destination MAC address: broadcast address
	fec_get_mac_address(arp.sender_mac); // Get MAC address from FEC
	arp.htype = htons(ARP_HW_TYPE);
	arp.ptype = htons(ARP_PROT_TYPE);
	arp.hlen = ARP_HW_ADD_LEN;
	arp.plen = ARP_PROT_ADD_LEN;
	arp.opcode = htons(ARP_REQUEST);

	/* ethernet frame */

	eth.protocol = htons(ETH_ARP);
	memset(eth.target_mac, 0xff, 6); // Set destination MAC address: broadcast address
	fec_get_mac_address(eth.sender_mac); // Get MAC address from FEC
	memcpy(eth_frame, &eth, ETH_HDRLEN); // First part is ethernet header
	memcpy((eth_frame + ETH_HDRLEN), &arp, ARP_HDRLEN); // Next part is an IPv4 header.
	fec_send(eth_frame, (ARP_HDRLEN + ETH_HDRLEN) * sizeof(uint8_t));
	free(eth_frame);
}

void network_send_arp_reply(uint8_t *target_ip, uint8_t *target_mac) {

	//printf("Sending ARP reply...\n");

	/* init structs size */

	arp_hdr_t arp;
	eth_hdr_t eth;
	uint8_t *eth_frame;
	eth_frame = (uint8_t *) malloc((ARP_HDRLEN + ETH_HDRLEN) * sizeof(uint8_t));
	memset(eth_frame, 0, IP_MAXPACKET);

	/* arp frame */
	struct ip_addr myip = ip_get_ip_address();
	arp.sender_ip[0] = myip.addrA;
	arp.sender_ip[1] = myip.addrB;
	arp.sender_ip[2] = myip.addrC;
	arp.sender_ip[3] = myip.addrD;
	arp.target_ip[0] = target_ip[0];
	arp.target_ip[1] = target_ip[1];
	arp.target_ip[2] = target_ip[2];
	arp.target_ip[3] = target_ip[3];
	arp.target_mac[0] = target_mac[0]; // Set destination MAC address
	arp.target_mac[1] = target_mac[1];
	arp.target_mac[2] = target_mac[2];
	arp.target_mac[3] = target_mac[3];
	arp.target_mac[4] = target_mac[4];
	arp.target_mac[5] = target_mac[5];
	fec_get_mac_address(arp.sender_mac); // Get MAC address from FEC
	arp.htype = htons(ARP_HW_TYPE);
	arp.ptype = htons(ARP_PROT_TYPE);
	arp.hlen = ARP_HW_ADD_LEN;
	arp.plen = ARP_PROT_ADD_LEN;
	arp.opcode = htons(ARP_REPLY);

	/* ethernet frame */

	eth.protocol = htons(ETH_ARP);
	eth.target_mac[0] = target_mac[0]; // Set destination MAC address
	eth.target_mac[1] = target_mac[1];
	eth.target_mac[2] = target_mac[2];
	eth.target_mac[3] = target_mac[3];
	eth.target_mac[4] = target_mac[4];
	eth.target_mac[5] = target_mac[5];
	fec_get_mac_address(eth.sender_mac); // Get MAC address from FEC
	memcpy(eth_frame, &eth, ETH_HDRLEN); // First part is ethernet header
	memcpy((eth_frame + ETH_HDRLEN), &arp, ARP_HDRLEN); // Next part is an IPv4 header.
	fec_send(eth_frame, (ARP_HDRLEN + ETH_HDRLEN) * sizeof(uint8_t));
	free(eth_frame);
}

void network_receive_frame() {
	eth_hdr_t *eth;
	arp_hdr_t *arp;
	icmp_hdr_t *icmp;
	ip_hdr_t *ip;
	uint8_t *ether_frame = (uint8_t*) malloc(IP_MAXPACKET * sizeof(uint8_t));
	memset(ether_frame, 0, IP_MAXPACKET);
	int count = 0;
	// Receive frames until ARP or ICMP frame
	int status = fec_recv(ether_frame, IP_MAXPACKET);
	if (status == 0) {
		while (((((ether_frame[12]) << 8) + ether_frame[13]) != ETH_ARP)
				&& ((((ether_frame[12]) << 8) + ether_frame[13]) != ETH_IP4)) {
			memset(ether_frame, 0, IP_MAXPACKET);
			fec_recv(ether_frame, IP_MAXPACKET);
			count++;
			//if(count == 10)
			kernel_reschedule();
		}
		eth = (eth_hdr_t*) (ether_frame);
		uint8_t my_mac[6];
		fec_get_mac_address(my_mac);
		struct ip_addr myip = ip_get_ip_address();
		if ((eth->target_mac[0] == my_mac[0] && eth->target_mac[1] == my_mac[1]
				&& eth->target_mac[2] == my_mac[2]
				&& eth->target_mac[3] == my_mac[3]
				&& eth->target_mac[4] == my_mac[4]
				&& eth->target_mac[5] == my_mac[5])
				|| (eth->target_mac[0] == 0xff && eth->target_mac[1] == 0xff
						&& eth->target_mac[2] == 0xff
						&& eth->target_mac[3] == 0xff
						&& eth->target_mac[4] == 0xff
						&& eth->target_mac[5] == 0xff)) { // only process useful frames
			if ((((ether_frame[12]) << 8) + ether_frame[13]) == ETH_ARP) {
				arp = (arp_hdr_t*) (ether_frame + ETH_HDRLEN);
				// received ARP REPLY from previous ARP REQUEST
				if ((ntohs (arp->opcode) == ARP_REPLY)
						&& arp->target_ip[0] == myip.addrA
						&& arp->target_ip[1] == myip.addrB
						&& arp->target_ip[2] == myip.addrC
						&& arp->target_ip[3] == myip.addrD) {
					uint32_t cache_size = ++(arpcache.cache_size);
					if (cache_size == ARP_CACHE_LEN)
						cache_size = 1;
					arpcache.ip[cache_size - 1][0] = arp->sender_ip[0];
					arpcache.ip[cache_size - 1][1] = arp->sender_ip[1];
					arpcache.ip[cache_size - 1][2] = arp->sender_ip[2];
					arpcache.ip[cache_size - 1][3] = arp->sender_ip[3];
					arpcache.mac[cache_size - 1][0] = arp->sender_mac[0];
					arpcache.mac[cache_size - 1][1] = arp->sender_mac[1];
					arpcache.mac[cache_size - 1][2] = arp->sender_mac[2];
					arpcache.mac[cache_size - 1][3] = arp->sender_mac[3];
					arpcache.mac[cache_size - 1][4] = arp->sender_mac[4];
					arpcache.mac[cache_size - 1][5] = arp->sender_mac[5];
					consolemanager_println("MAC added in table");
					//consolemanager_println("Received ARP reply");
				}
				// received ARP REQUEST
				else if ((ntohs (arp->opcode) == ARP_REQUEST)
						&& arp->target_ip[0] == myip.addrA && arp->target_ip[1] == myip.addrB
						&& arp->target_ip[2] == myip.addrC && arp->target_ip[3] == myip.addrD) {
					//printf("Received ARP request\n");
					network_send_arp_reply(arp->sender_ip, arp->sender_mac);
				}
			} else if ((((ether_frame[12]) << 8) + ether_frame[13]) == ETH_IP4) {
				ip = (ip_hdr_t*) (ether_frame + ETH_HDRLEN);
				icmp = (icmp_hdr_t*) (ether_frame + ETH_HDRLEN + IP4_HDRLEN);
				// receive ICMP ECHO REPLY for our address
				struct ip_addr myip = ip_get_ip_address();
				if (icmp->ping_icmp.icmp_type == ICMP_ECHO_REPLY
						&& ip->ip_dst[0] == myip.addrA
						&& ip->ip_dst[1] == myip.addrB
						&& ip->ip_dst[2] == myip.addrC
						&& ip->ip_dst[3] == myip.addrD) {
					char out[50];
					sprintf(out, "Ping of %u.%u.%u.%u successful\n",
							ip->ip_src[0], ip->ip_src[1], ip->ip_src[2],
							ip->ip_src[3]);
					//consolemanager_println(out);
				} else if (icmp->ping_icmp.icmp_type == ICMP_ECHO
						&& ip->ip_dst[0] == myip.addrA
						&& ip->ip_dst[1] == myip.addrB
						&& ip->ip_dst[2] == myip.addrC
						&& ip->ip_dst[3] == myip.addrD) {
					//consolemanager_println("Received ICMP request\n");
					network_send_icmp_reply(ip->ip_src, ether_frame + 6, icmp,
							ip,
							ether_frame + ETH_HDRLEN + IP4_HDRLEN + ICMP_HDRLEN);
				}
			}
		}
		free(ether_frame);
	}
}

int network_ping(struct ip_addr dest_ip_addr) {

	consolemanager_println("Sending ICMP request...\n");

	int datalen, *ip_flags;
	struct eth_hdr eth;
	struct icmp_ping icmphdr;
	struct ip_hdr iphdr;
	uint8_t *eth_frame;
	uint8_t *data;
	uint8_t dest_mac_array[6] = { 255, 255, 255, 255, 255, 255 };

	// Allocate memory for various arrays.
	// Maximum ICMP payload size = 65535 - ETH header (14 bytes) - IPv4 header (20 bytes) - ICMP header (8 bytes)
	eth_frame = (uint8_t *) malloc(IP_MAXPACKET * sizeof(uint8_t));
	memset(eth_frame, 0, IP_MAXPACKET);

	data = (uint8_t *) malloc(
			(IP_MAXPACKET - IP4_HDRLEN - ICMP_HDRLEN - ETH_HDRLEN));
	memset(data, 0, (IP_MAXPACKET - IP4_HDRLEN - ICMP_HDRLEN - ETH_HDRLEN));

	ip_flags = (int *) malloc(4 * sizeof(int));
	memset(ip_flags, 0, 4 * sizeof(int));

	int mac_found = 0;

	// Check for MAC address knowledge
	for (uint32_t i = 0; i < arpcache.cache_size; i++) {
		if (arpcache.ip[i][0] == dest_ip_addr.addrA
				&& arpcache.ip[i][1] == dest_ip_addr.addrB
				&& arpcache.ip[i][2] == dest_ip_addr.addrC
				&& arpcache.ip[i][3] == dest_ip_addr.addrD) {
			//consolemanager_println("MAC already in table");
			mac_found = 1;
			dest_mac_array[0] = arpcache.mac[i][0];
			dest_mac_array[1] = arpcache.mac[i][1];
			dest_mac_array[2] = arpcache.mac[i][2];
			dest_mac_array[3] = arpcache.mac[i][3];
			dest_mac_array[4] = arpcache.mac[i][4];
			dest_mac_array[5] = arpcache.mac[i][5];
			break;
		}
	}

	// Send ARP request if no MAC address knowledge
	if (mac_found == 0) {
		uint32_t crt_cache_size = arpcache.cache_size;
		int timeout = 0;
		while (crt_cache_size == arpcache.cache_size) { // no new entry in arp table
			if (timeout == 50) {
				return 1;
			}
			timeout += 1;
			network_send_arp_request(dest_ip_addr.addrA, dest_ip_addr.addrB,
					dest_ip_addr.addrC, dest_ip_addr.addrD);
			kernel_reschedule();
		}
		for (uint32_t i = 0; i < arpcache.cache_size; i++) {
			if (arpcache.ip[i][0] == dest_ip_addr.addrA
					&& arpcache.ip[i][1] == dest_ip_addr.addrB
					&& arpcache.ip[i][2] == dest_ip_addr.addrC
					&& arpcache.ip[i][3] == dest_ip_addr.addrD) {
				dest_mac_array[0] = arpcache.mac[i][0];
				dest_mac_array[1] = arpcache.mac[i][1];
				dest_mac_array[2] = arpcache.mac[i][2];
				dest_mac_array[3] = arpcache.mac[i][3];
				dest_mac_array[4] = arpcache.mac[i][4];
				dest_mac_array[5] = arpcache.mac[i][5];
				break;
			}
		}
	}

	// ICMP data
	datalen = 4;
	data[0] = 'T';
	data[1] = 'e';
	data[2] = 's';
	data[3] = 't';

	// Ethernet header

	eth.protocol = htons(ETH_IP4);
	eth.target_mac[0] = dest_mac_array[0];
	eth.target_mac[1] = dest_mac_array[1];
	eth.target_mac[2] = dest_mac_array[2];
	eth.target_mac[3] = dest_mac_array[3];
	eth.target_mac[4] = dest_mac_array[4];
	eth.target_mac[5] = dest_mac_array[5];
	fec_get_mac_address((eth).sender_mac);

	// IPv4 header

	iphdr.ip_version = 4; // Internet Protocol version (4 bits): IPv4
	iphdr.ip_hdr_len = IP4_HDRLEN / sizeof(unsigned long int); // IPv4 header length (4 bits): Number of 32-bit words in header = 5
	iphdr.ip_tos = 0; // Type of service (8 bits)
	iphdr.ip_len = htons(IP4_HDRLEN + ICMP_HDRLEN + datalen); // Total length of datagram (16 bits): IP header + ICMP header + ICMP data
	iphdr.ip_id = htons(0); // ID sequence number (16 bits): unused, since single datagram

	// Flags, and Fragmentation offset (3, 13 bits): 0 since single datagram

	ip_flags[0] = 0; // Zero (1 bit)
	ip_flags[1] = 0; // Do not fragment flag (1 bit)
	ip_flags[2] = 0; // More fragments following flag (1 bit)
	ip_flags[3] = 0; // Fragmentation offset (13 bits)

	iphdr.ip_off = htons(
			(ip_flags[0] << 15) + (ip_flags[1] << 14) + (ip_flags[2] << 13)
					+ ip_flags[3]);

	iphdr.ip_ttl = 255; // Time-to-Live (8 bits): default to maximum value
	iphdr.ip_proto = IP_ICMP_PROTOCOL; // Transport layer protocol (8 bits): 1 for ICMP
	struct ip_addr myip = ip_get_ip_address();
	iphdr.ip_src[0] = myip.addrA; // Source IPv4 address (32 bits), default : 192.168.0.10
	iphdr.ip_src[1] = myip.addrB;
	iphdr.ip_src[2] = myip.addrC;
	iphdr.ip_src[3] = myip.addrD;
	iphdr.ip_dst[0] = dest_ip_addr.addrA; // Destination IPv4 address (32 bits)
	iphdr.ip_dst[1] = dest_ip_addr.addrB;
	iphdr.ip_dst[2] = dest_ip_addr.addrC;
	iphdr.ip_dst[3] = dest_ip_addr.addrD;
	iphdr.ip_chk = 0; // IPv4 header checksum (16 bits): set to 0 when calculating checksum
	iphdr.ip_chk = checksum((uint16_t *) &iphdr, IP4_HDRLEN);

	// ICMP header

	(icmphdr.ping_icmp).icmp_type = ICMP_ECHO; // Message Type (8 bits): echo request
	(icmphdr.ping_icmp).icmp_code = 0; // Message Code (8 bits): echo request
	icmphdr.ping_id = htons(1000); // Identifier (16 bits): pick a number
	icmphdr.ping_seq = htons(0); // Sequence Number (16 bits): starts at 0
	(icmphdr.ping_icmp).icmp_chk = 0; // ICMP header checksum (16 bits): set to 0 when calculating checksum

	// Prepare packet.

	memcpy(eth_frame, &eth, ETH_HDRLEN); // First part is ethernet header
	memcpy((eth_frame + ETH_HDRLEN), &iphdr, IP4_HDRLEN); // Next part is an IPv4 header.
	memcpy((eth_frame + IP4_HDRLEN + ETH_HDRLEN), &icmphdr, ICMP_HDRLEN); // Next part is an ICMP header.
	memcpy((eth_frame + IP4_HDRLEN + ICMP_HDRLEN + ETH_HDRLEN), data, datalen); // Finally, add the ICMP data.

	// Calculate ICMP header checksum
	(icmphdr.ping_icmp).icmp_chk = checksum(
			(uint16_t *) (eth_frame + ETH_HDRLEN + IP4_HDRLEN),
			ICMP_HDRLEN + datalen);
	memcpy((eth_frame + IP4_HDRLEN + ETH_HDRLEN), &icmphdr, ICMP_HDRLEN);

	// Send the frame through FEC
	fec_send(eth_frame,
			(ETH_HDRLEN + IP4_HDRLEN + ICMP_HDRLEN + datalen)
					* sizeof(uint8_t));
	free(eth_frame);
	free(ip_flags);
	free(data);
	return 0;
}

void network_send_icmp_reply(uint8_t *target_ip, uint8_t *target_mac,
		icmp_hdr_t *icmp, ip_hdr_t *ip, uint8_t *data) {

	consolemanager_println("Sending ICMP reply...\n");

	int datalen;
	struct eth_hdr eth;
	struct icmp_ping icmphdr;
	struct ip_hdr iphdr;
	uint8_t *eth_frame;
	datalen = 4;

	// Allocate memory for various arrays.
	// Maximum ICMP payload size = 65535 - ETH header (14 bytes) - IPv4 header (20 bytes) - ICMP header (8 bytes)
	eth_frame = (uint8_t *) malloc(
			(ETH_HDRLEN + IP4_HDRLEN + ICMP_HDRLEN + datalen)
					* sizeof(uint8_t));
	memset(eth_frame, 0, (ETH_HDRLEN + IP4_HDRLEN + ICMP_HDRLEN + datalen));

	// ICMP data
	datalen = htons(ip->ip_len) - IP4_HDRLEN - ICMP_HDRLEN;

	// Ethernet header

	eth.protocol = htons(ETH_IP4);
	eth.target_mac[0] = target_mac[0]; // Macbook Pro : 10:9a:dd:52:07:38
	eth.target_mac[1] = target_mac[1];
	eth.target_mac[2] = target_mac[2];
	eth.target_mac[3] = target_mac[3];
	eth.target_mac[4] = target_mac[4];
	eth.target_mac[5] = target_mac[5];
	fec_get_mac_address((eth).sender_mac);

	// IPv4 header

	iphdr.ip_version = 4; // Internet Protocol version (4 bits): IPv4
	iphdr.ip_hdr_len = IP4_HDRLEN / sizeof(unsigned long int); // IPv4 header length (4 bits): Number of 32-bit words in header = 5
	iphdr.ip_tos = 0; // Type of service (8 bits)
	iphdr.ip_len = ip->ip_len; // Total length of datagram (16 bits): IP header + ICMP header + ICMP data
	iphdr.ip_id = ip->ip_id; // ID sequence number (16 bits): unused, since single datagram

	iphdr.ip_off = ip->ip_off; // Flags, and Fragmentation offset (3, 13 bits): 0 since single datagram

	iphdr.ip_ttl = ip->ip_ttl; // Time-to-Live (8 bits): default to maximum value
	iphdr.ip_proto = IP_ICMP_PROTOCOL; // Transport layer protocol (8 bits): 1 for ICMP
	struct ip_addr myip = ip_get_ip_address();
	iphdr.ip_src[0] = myip.addrA; // Source IPv4 address (32 bits), default : 192.168.0.10
	iphdr.ip_src[1] = myip.addrB;
	iphdr.ip_src[2] = myip.addrC;
	iphdr.ip_src[3] = myip.addrD;
	iphdr.ip_dst[0] = target_ip[0]; // Destination IPv4 address (32 bits)
	iphdr.ip_dst[1] = target_ip[1];
	iphdr.ip_dst[2] = target_ip[2];
	iphdr.ip_dst[3] = target_ip[3];
	iphdr.ip_chk = 0; // IPv4 header checksum (16 bits): set to 0 when calculating checksum
	iphdr.ip_chk = checksum((uint16_t *) &iphdr, IP4_HDRLEN);

	// ICMP header

	(icmphdr.ping_icmp).icmp_type = ICMP_ECHO_REPLY; // Message Type (8 bits): echo request
	(icmphdr.ping_icmp).icmp_code = 0; // Message Code (8 bits): echo request
	icmphdr.ping_id = icmp->ping_id; // Identifier (16 bits): pick a number
	icmphdr.ping_seq = icmp->ping_seq; // Sequence Number (16 bits): starts at 0
	(icmphdr.ping_icmp).icmp_chk = 0; // ICMP header checksum (16 bits): set to 0 when calculating checksum

	// Prepare packet.

	memcpy(eth_frame, &eth, ETH_HDRLEN); // First part is ethernet header
	memcpy((eth_frame + ETH_HDRLEN), &iphdr, IP4_HDRLEN); // Next part is an IPv4 header.
	memcpy((eth_frame + IP4_HDRLEN + ETH_HDRLEN), &icmphdr, ICMP_HDRLEN); // Next part is an ICMP header.
	memcpy((eth_frame + IP4_HDRLEN + ICMP_HDRLEN + ETH_HDRLEN), data, datalen); // Finally, add the ICMP data.

	// Calculate ICMP header checksum
	(icmphdr.ping_icmp).icmp_chk = checksum(
			(uint16_t *) (eth_frame + ETH_HDRLEN + IP4_HDRLEN),
			ICMP_HDRLEN + datalen);
	memcpy((eth_frame + IP4_HDRLEN + ETH_HDRLEN), &icmphdr, ICMP_HDRLEN);

	// Send the frame through FEC
	fec_send(eth_frame, ETH_HDRLEN + IP4_HDRLEN + ICMP_HDRLEN + datalen);
	free(eth_frame);
}

