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
  int type;
  char *payload;
  int payload_size;
};

// The socket fd
int sockfd;

/**
 * Function to set packet type as ECHO
 */
void set_echo_type(struct icmp_packet *packet);

/**
 * Function to open a socket for icmp
 */
void open_icmp_socket();

/**
 * Function to bind the socket to INADDR_ANY
 */
void bind_icmp_socket();

/**
 * Function to send ICMP Packet
 */
void send_icmp_packet(struct icmp_packet *packet_details);

/**
 * Function to receive ICMP Packet
 */
void receive_icmp_packet(struct icmp_packet *packet_details);

/**
 * Function to close the icmp socket
 */
void close_icmp_socket();

#endif