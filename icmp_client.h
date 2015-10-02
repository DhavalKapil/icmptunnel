/**
 *  icmp_client.h
 */

#include "icmp.h"

#ifndef icmp_client_gaurd
#define icmp_client_gaurd

/**
 * Function to initialize client
 */
void initialize_client();

/**
 * Function to send ICMP Packet
 */
void send_packet(struct icmp_packet *packet_details);

#endif
