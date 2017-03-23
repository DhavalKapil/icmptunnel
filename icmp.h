/**
 *  icmp.h
 */
 
#ifndef icmp_guard
#define icmp_guard

// Maximum transmission unit
#define MTU 1472

struct icmp_packet
{
  char src_addr[100];
  char dest_addr[100];
  int type;
  char *payload;
  int payload_size;
};

/**
 * Function to set packet type as ECHO
 */
void set_echo_type(struct icmp_packet *packet);

/**
 * Function to set packet type as REPLY
 */
void set_reply_type(struct icmp_packet *packet);

/**
 * Function to open a socket for icmp
 */
int open_icmp_socket();

/**
 * Function to bind the socket to INADDR_ANY
 */
void bind_icmp_socket(int sock_fd);

/**
 * Function to send ICMP Packet
 */
void send_icmp_packet(int sock_fd, struct icmp_packet *packet_details);

/**
 * Function to receive ICMP Packet
 */
void receive_icmp_packet(int sock_fd, struct icmp_packet *packet_details);

/**
 * Function to close the icmp socket
 */
void close_icmp_socket(int sock_fd);

#endif
