/**
 *  icmp.c
 */

#include <stdint.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>

// The socket fd
int sockfd;

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
 * Function to open the socket
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
 * Function to fill up common headers for IP and ICMP
 */
void prepare_headers(struct iphdr *ip, struct icmphdr *icmp)
{
  memset(ip, 0, sizeof(ip);
  memset(icmp, 0, sizeof(icmp);

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
  uint32_t src_addr;
  uint32_t dest_addr;

  struct iphdr *ip;
  struct icmphdr *icmp;

  int packet_size;
  char *packet;

  src_addr = inet_addr(packet_details->src_addr);
  dest_addr = inet_addr(packet_details->dest_addr);

  packet_size = sizeof(struct iphdr) + sizeof(struct icmphdr) + packet_details->payload_size;
  packet = (char *)malloc(packet_size);

  if (packet == NULL) {
    printf("No memory available\n");
    close_socket();
    exit(-1);
  }

  prepare_headers(ip, icmp);

  ip->tot_len = htons(packet_size);

  icmp
}
