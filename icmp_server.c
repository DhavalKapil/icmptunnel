/**
 *  icmp_server.c
 */

#include "icmp_server.h"
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Function to initialize the server
 */
void initialize_server()
{
  struct sockaddr_in servaddr;

  open_socket();

  memset(&servaddr, 0, sizeof(struct sockaddr_in));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) == -1){
    printf("Unable to bind\n");
    exit(-1);
  }
}

/**
 * Function to receive a packet
 */
void receive_packet(struct icmp_packet *packet_details)
{
  struct sockaddr_in src_addr;
  struct sockaddr_in dest_addr;

  struct iphdr *ip;
  struct icmphdr *icmp;
  char *icmp_payload;

  int packet_size;
  char *packet;

  int src_addr_size;

  packet = (char *)malloc(MTU);
  memset(packet, 0, MTU);

  src_addr_size = sizeof(struct sockaddr_in);
  
  while (1) {
    packet_size = recvfrom(sockfd, packet, MTU, 0, (struct sockaddr *)&(src_addr), &src_addr_size);
    ip = (struct iphdr *)packet;
    icmp = (struct icmphdr *)(packet + sizeof(struct iphdr));

    if (icmp->type == ICMP_ECHO)
      break;
  }

  icmp_payload = (char *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr));

  inet_ntop(AF_INET, &(ip->saddr), packet_details->src_addr, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &(ip->daddr), packet_details->dest_addr, INET_ADDRSTRLEN);
  packet_details->payload = icmp_payload;
  packet_details->payload_size = packet_size - sizeof(struct iphdr) - sizeof(struct icmphdr);

}