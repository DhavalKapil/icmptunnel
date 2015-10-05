/**
 *  icmp.h
 */

#include <stdint.h>
 
#ifndef icmp_gaurd
#define icmp_gaurd

// Maximum transmission unit
#define MTU 1500

struct icmp_packet
{
  char src_addr[100];
  char dest_addr[100];
  char *payload;
  int payload_size;
};

// The socket fd
int sockfd;

/**
 * Function to calculate checksum
 */
uint16_t in_cksum(uint16_t *addr, int len);

/**
 * Function to open socket
 */
void open_socket();

/**
 * Function to close socket
 */
void close_socket();

#endif