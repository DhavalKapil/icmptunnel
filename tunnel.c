/**
 *  tunnel.c
 */

#include "icmp.h"
#include "tunnel.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <netdb.h>
#include <pwd.h>
#include <pthread.h>
#include <arpa/inet.h>

/**
 * Function to allocate a tunnel
 */
int tun_alloc(char *dev, int flags)
{
  struct ifreq ifr;
  int tun_fd, err;
  char *clonedev = "/dev/net/tun";

  tun_fd = open(clonedev, O_RDWR);

  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = flags;

  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if ((err=ioctl(tun_fd, TUNSETIFF, (void *)&ifr)) < 0) {
    close(tun_fd);
    printf("Error: %d\n", err);
    exit(-1);
  }

  strcpy(dev, ifr.ifr_name);

  return tun_fd;
}

/**
 * Function to read from a tunnel
 */
int tun_read(int tun_fd, char *buffer, int length)
{
  return write(tun_fd, buffer, length);
}

/**
 * Function to write to a tunnel
 */
int tun_write(int tun_fd, char *buffer, int length)
{
  return read(tun_fd, buffer, length);
}

/**
 * Function to run the tunnel
 */
void run_tunnel(int tun_fd, char *dest, int server)
{
  struct icmp_packet packet;

  fd_set fs;

  open_icmp_socket();

  if (server) {
    bind_icmp_socket();
  }

  while (1) {
    FD_ZERO(&fs);
    FD_SET(tun_fd, &fs);
    FD_SET(sockfd, &fs);

    select(tun_fd>sockfd?tun_fd+1:sockfd+1, &fs, NULL, NULL, NULL);

    if (FD_ISSET(tun_fd, &fs)) {
      // Reading data from tun device and sending ICMP packet

      // Preparing ICMP packet to be sent
      memset(&packet, 0, sizeof(struct icmp_packet));
      strcpy(packet.dest_addr, dest);
      if(server) {
        set_reply_type(&packet);
      }
      else {
        set_echo_type(&packet);
      }
      packet.payload = malloc(MTU);
      packet.payload_size  = tun_read(tun_fd, packet.payload, MTU);
      if(packet.payload_size  == -1) {
        printf("Error while reading from tun device\n");
        exit(-1);
      }

      // Sending ICMP packet
      send_icmp_packet(&packet);

      free(packet.payload);
    }

    if (FD_ISSET(sockfd, &fs)) {
      // Reading data from remote socket and sending to tun device

      // Getting ICMP packet
      memset(&packet, 0, sizeof(struct icmp_packet));
      receive_icmp_packet(&packet);

      // Writing out to tun device
      tun_write(tun_fd, packet.payload, packet.payload_size);

      if (dest == NULL) {
        dest = malloc(100);
        strcpy(dest, packet.src_addr);
      }
    }
  }

}