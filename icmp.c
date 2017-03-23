/**
 *  icmp.c
 */

#include "icmp.h"
#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Function to calculate checksum
 */
uint16_t in_cksum(uint16_t *addr, int len);

/**
 * Function to fill up common headers for IP and ICMP
 */
void prepare_headers(struct iphdr *ip, struct icmphdr *icmp);

/**
 * Function to set packet type as ECHO
 */
void set_echo_type(struct icmp_packet *packet)
{
  packet->type = ICMP_ECHO;
}

/**
 * Function to set packet type as REPLY
 */
void set_reply_type(struct icmp_packet *packet)
{
  packet->type = ICMP_ECHOREPLY;
}

/**
 * Function to open a socket for icmp
 */
int open_icmp_socket()
{
  int sock_fd, on = 1;

  sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

  if (sock_fd == -1) {
    perror("Unable to open ICMP socket\n");
    exit(EXIT_FAILURE);
  }
  
  // Providing IP Headers
  if (setsockopt(sock_fd, IPPROTO_IP, IP_HDRINCL, (const char *)&on, sizeof(on)) == -1) {
    perror("Unable to set IP_HDRINCL socket option\n");
    exit(EXIT_FAILURE);
  }

  return sock_fd;
}

/**
 * Function to bind the socket to INADDR_ANY
 */
void bind_icmp_socket(int sock_fd)
{
  struct sockaddr_in servaddr;

  // Initializing servaddr to bind to all interfaces
  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  // binding the socket
  if (bind(sock_fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) == -1) {
    perror("Unable to bind\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * Function to send ICMP Packet
 */
void send_icmp_packet(int sock_fd, struct icmp_packet *packet_details)
{
  // Source and destination IPs
  struct in_addr src_addr;
  struct in_addr dest_addr;

  struct iphdr *ip;
  struct icmphdr *icmp;
  char *icmp_payload;

  int packet_size;
  char *packet;

  struct sockaddr_in servaddr;

  inet_pton(AF_INET, packet_details->src_addr, &src_addr);
  inet_pton(AF_INET, packet_details->dest_addr, &dest_addr);

  packet_size = sizeof(struct iphdr) + sizeof(struct icmphdr) + packet_details->payload_size;
  packet = calloc(packet_size, sizeof(uint8_t));
  if (packet == NULL) {
    perror("No memory available\n");
    close_icmp_socket(sock_fd);
    exit(EXIT_FAILURE);
  }

  // Initializing header and payload pointers
  ip = (struct iphdr *)packet;
  icmp = (struct icmphdr *)(packet + sizeof(struct iphdr));
  icmp_payload = (char *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr));

  prepare_headers(ip, icmp);

  ip->tot_len = htons(packet_size);
  ip->saddr = src_addr.s_addr;
  ip->daddr = dest_addr.s_addr;

  memcpy(icmp_payload, packet_details->payload, packet_details->payload_size);

  icmp->type = packet_details->type;
  icmp->checksum = 0;
  icmp->checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr) + packet_details->payload_size);

  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = dest_addr.s_addr;

  // Sending the packet
  sendto(sock_fd, packet, packet_size, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));

  free(packet);
}

/**
 * Function to receive an ICMP packet
 */
void receive_icmp_packet(int sock_fd, struct icmp_packet *packet_details)
{
  struct sockaddr_in src_addr;
  //struct sockaddr_in dest_addr;

  struct iphdr *ip;
  struct icmphdr *icmp;
  char *icmp_payload;

  int packet_size;
  char *packet;

  socklen_t src_addr_size;
  int enc_MTU; //encapsulated MTU

  enc_MTU = MTU + sizeof(struct iphdr) + sizeof(struct icmphdr);

  packet = calloc(enc_MTU, sizeof(uint8_t));
  if (packet == NULL) {
    perror("No memory available\n");
    close_icmp_socket(sock_fd);
    exit(-1);
  }

  src_addr_size = sizeof(struct sockaddr_in);
  
  // Receiving packet
  packet_size = recvfrom(sock_fd, packet, enc_MTU, 0, (struct sockaddr *)&(src_addr), &src_addr_size);

  ip = (struct iphdr *)packet;
  icmp = (struct icmphdr *)(packet + sizeof(struct iphdr));
  icmp_payload = (char *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr));

  // Filling up packet_details
  inet_ntop(AF_INET, &(ip->saddr), packet_details->src_addr, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &(ip->daddr), packet_details->dest_addr, INET_ADDRSTRLEN);
  packet_details->type = icmp->type;
  packet_details->payload_size = packet_size - sizeof(struct iphdr) - sizeof(struct icmphdr);
  packet_details->payload = calloc(packet_details->payload_size, sizeof(uint8_t));
  if (packet_details->payload == NULL) {
    perror("No memory available\n");
    close_icmp_socket(sock_fd);
    exit(-1);
  }

  memcpy(packet_details->payload, icmp_payload, packet_details->payload_size);

  free(packet);
}

/**
 * Function to close the icmp socket
 */
void close_icmp_socket(int sock_fd)
{
  close(sock_fd);
}

/**
 * Function to calculate checksum
 */
uint16_t in_cksum(uint16_t *addr, int len)
{
  int nleft = len;
  uint32_t sum = 0;
  uint16_t *w = addr;
  uint16_t answer = 0;

  // Adding 16 bits sequentially in sum
  while (nleft > 1) {
    sum += *w;
    nleft -= 2;
    w++;
  }

  // If an odd byte is left
  if (nleft == 1) {
    *(unsigned char *) (&answer) = *(unsigned char *) w;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;

  return answer;
}

/**
 * Function to fill up common headers for IP and ICMP
 */
void prepare_headers(struct iphdr *ip, struct icmphdr *icmp)
{
  ip->version = 4;
  ip->ihl = 5;
  ip->tos = 0;
  ip->id = rand();
  ip->frag_off = 0;
  ip->ttl = 255;
  ip->protocol = IPPROTO_ICMP;

  icmp->code = 0;
  icmp->un.echo.sequence = rand();
  icmp->un.echo.id = rand();
  icmp->checksum = 0;   
}
