/**
 *  icmp_server.h
 */

#include "icmp.h"

#ifndef icmp_server_gaurd
#define icmp_server_gaurd

/**
 * Function to initialize the server
 */
void initialize_server();

/**
 * Function to receive a packet
 */
void receive_packet(struct icmp_packet *packet_details);

#endif