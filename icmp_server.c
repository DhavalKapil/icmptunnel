/**
 *  icmp_server.c
 */

#include "icmp_server.h"
#include <stdint.h>
#include <string.h>
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
  char *src_addr;
  char *dest_addr;

  struct iphdr *ip;
  char *icmp_payload;

  int packet_size;
  char *packet;

  struct sockaddr_in clntaddr;
  int clntaddr_size;

  packet = (char *)malloc(MTU);
  memset(packet, 0, MTU);

  clntaddr_size = sizeof(struct sockaddr_in);
  
  while(1){
  packet_size = recvfrom(sockfd, packet, MTU, 0, (struct sockaddr *)&(clntaddr), &clntaddr_size);
  ip = (struct iphdr *)packet;
  if(ip->protocol == IPPROTO_ICMP)
    break;
  }

  icmp_payload = (char *)(packet + sizeof(struct iphdr) + sizeof(struct icmphdr));

  packet_details->src_addr = inet_ntoa(ip->saddr);
  packet_details->dest_addr = inet_ntoa(ip->daddr);
  packet_details->payload = icmp_payload;
  packet_details->payload_size = packet_size - sizeof(struct iphdr) - sizeof(struct icmphdr);

}