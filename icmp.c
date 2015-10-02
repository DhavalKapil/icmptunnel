/**
 *  icmp.c
 */

#include "icmp.h"
#include <stdint.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Function to open_socket
 */
void open_socket()
{
  int on = 1;

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

  // Providing IP Headers
  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, (const char *)&on, sizeof(on)) == -1) {
    printf("Unable to set IP_HDRINCL socket option\n");
    exit(-1);
  }
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
 * Function to close socket
 */
void close_socket()
{
  close(sockfd);
}