/**
 *  icmp_client.c
 */

#include "icmp_client.h"
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Function to initialize client
 */
void initialize_client()
{
  open_socket();
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

  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->un.echo.sequence = rand();
  icmp->un.echo.id = rand();
  icmp->checksum = 0;   
}

/**
 * Function to send ICMP Packet
 */
void send_packet(struct icmp_packet *packet_details)
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
  packet = (char *)malloc(packet_size);
  if (packet == NULL) {
    printf("No memory available\n");
    close_socket();
    exit(-1);
  }
  memset(packet, 0, packet_size);

  ip = (struct iphdr *)packet;
  icmp = (struct icmphdr *)(packet + sizeof(struct iphdr));
  icmp_payload = (char *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr));

  prepare_headers(ip, icmp);

  ip->tot_len = htons(packet_size);
  ip->saddr = src_addr.s_addr;
  ip->daddr = dest_addr.s_addr;

  memcpy(icmp_payload, packet_details->payload, packet_details->payload_size);

  icmp->checksum = 0;
  icmp->checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr) + packet_details->payload_size);

  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = dest_addr.s_addr;

  sendto(sockfd, packet, packet_size, 0, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));

  free(packet);
}