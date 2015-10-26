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
  printf("[DEBUG] Allocatating tunnel\n");

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

  printf("[DEBUG] Allocatating tunnel2");
  //strcpy(dev, ifr.ifr_name);
  printf("[DEBUG] Created tunnel %s\n", dev);

  return tun_fd;
}

/**
 * Function to read from a tunnel
 */
int tun_read(int tun_fd, char *buffer, int length)
{
  printf("[DEBUG] Reading from tunnel\n");
  return write(tun_fd, buffer, length);
}

/**
 * Function to write to a tunnel
 */
int tun_write(int tun_fd, char *buffer, int length)
{
  printf("[DEBUG] Writing to tunnel\n");
  return read(tun_fd, buffer, length);
}

/**
 * Function to run the tunnel
 */
void run_tunnel(char *dest, int server)
{
  struct icmp_packet packet;
  int tun_fd;

  fd_set fs;

  tun_fd = tun_alloc("tun0", IFF_TUN);

  printf("[DEBUG] Starting tunnel - Dest: %s, Server: %d\n", dest, server);
  printf("[DEBUG] Opening ICMP socket\n");
  open_icmp_socket();

  if (server) {
    printf("[DEBUG] Binding ICMP socket\n");
    bind_icmp_socket();
  }

  while (1) {
    FD_ZERO(&fs);
    FD_SET(tun_fd, &fs);
    FD_SET(sockfd, &fs);

    select(tun_fd>sockfd?tun_fd+1:sockfd+1, &fs, NULL, NULL, NULL);

    if (FD_ISSET(tun_fd, &fs)) {
      printf("[DEBUG] Data needs to be readed from tun device\n");
      // Reading data from tun device and sending ICMP packet

      printf("[DEBUG] Preparing ICMP packet to be sent\n");
      // Preparing ICMP packet to be sent
      memset(&packet, 0, sizeof(struct icmp_packet));
      printf("[DEBUG] Destination address: %s\n", dest);
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

      printf("[DEBUG] Sending ICMP packet with payload_size: %d, payload: %s\n", packet.payload_size, packet.payload);
      // Sending ICMP packet
      send_icmp_packet(&packet);

      free(packet.payload);
    }

    if (FD_ISSET(sockfd, &fs)) {
      printf("[DEBUG] Received ICMP packet\n");
      // Reading data from remote socket and sending to tun device

      // Getting ICMP packet
      memset(&packet, 0, sizeof(struct icmp_packet));
      receive_icmp_packet(&packet);

      printf("[DEBUG] Read ICMP packet with src: %s, dest: %s, payload_size: %d, payload: %s\n", packet.src_addr, packet.dest_addr, packet.payload_size, packet.payload);
      // Writing out to tun device
      tun_write(tun_fd, packet.payload, packet.payload_size);

      if (dest == NULL) {
        dest = malloc(100);
        printf("[DEBUG] Src address being copied: %s\n", packet.src_addr);
        strcpy(dest, packet.src_addr);
      }
    }
  }

}