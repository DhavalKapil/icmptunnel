/**
 *  icmp.h
 */

#include <stdint.h>
 
#ifndef icmp_gaurd
#define icmp_gaurd

struct icmp_packet
{
  char *src_addr;
  char *dest_addr;
  char *payload;
  int payload_size;
};

/**
 * Function to calculate checksum
 */
uint16_t in_cksum(uint16_t *addr, int len);

/**
 * Function to open the socket
 */
void open_socket();

/**
 * Function to close the socket
 */
void close_socket();

/**
 * Function to send ICMP Packet
 */
void send_packet(struct icmp_packet *packet_details);

#endif