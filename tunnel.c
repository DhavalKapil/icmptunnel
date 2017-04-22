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
#include <sys/wait.h>
#include <fcntl.h>
#include <netdb.h>
#include <pwd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>


#define DEFAULT_ROUTE   "0.0.0.0"

/**
 * Function to allocate a tunnel
 */
int tun_alloc(char *dev, int flags)
{
  struct ifreq ifr;
  int tun_fd, err;
  char *clonedev = "/dev/net/tun";
  printf("[DEBUG] Allocating tunnel\n");

  tun_fd = open(clonedev, O_RDWR);

  if(tun_fd == -1) {
    perror("Unable to open clone device\n");
    exit(EXIT_FAILURE);
  }
  
  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = flags;

  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if ((err=ioctl(tun_fd, TUNSETIFF, (void *)&ifr)) < 0) {
    close(tun_fd);
    fprintf(stderr, "Error returned by ioctl(): %s\n", strerror(err));
    perror("Error in tun_alloc()\n");
    exit(EXIT_FAILURE);
  }

  printf("[DEBUG] Allocatating tunnel2");

  printf("[DEBUG] Created tunnel %s\n", dev);

  return tun_fd;
}

/**
 * Function to read from a tunnel
 */
int tun_read(int tun_fd, char *buffer, int length)
{
  int bytes_read;
  printf("[DEBUG] Reading from tunnel\n");
  bytes_read = read(tun_fd, buffer, length);

  if (bytes_read == -1) {
    perror("Unable to read from tunnel\n");
    exit(EXIT_FAILURE);
  }
  else {
    return bytes_read;
  }
}

/**
 * Function to write to a tunnel
 */
int tun_write(int tun_fd, char *buffer, int length)
{
  int bytes_written;
  printf("[DEBUG] Writing to tunnel\n");
  bytes_written = write(tun_fd, buffer, length);

  if (bytes_written == -1) {
    perror("Unable to write to tunnel\n");
    exit(EXIT_FAILURE);
  }
  else {
    return bytes_written;
  }
}

/**
 * Function to configure the network
 */
void configure_network(int server)
{
  int pid, status;
  char path[100];
  char *const args[] = {path, NULL};

  if (server) {
    if (sizeof(SERVER_SCRIPT) > sizeof(path)){
      perror("Server script path is too long\n");
      exit(EXIT_FAILURE);
    }
    strncpy(path, SERVER_SCRIPT, strlen(SERVER_SCRIPT) + 1);
  }
  else {
    if (sizeof(CLIENT_SCRIPT) > sizeof(path)){
      perror("Client script path is too long\n");
      exit(EXIT_FAILURE);
    }
    strncpy(path, CLIENT_SCRIPT, strlen(CLIENT_SCRIPT) + 1);
  }

  pid = fork();

  if (pid == -1) {
    perror("Unable to fork\n");
    exit(EXIT_FAILURE);
  }
  
  if (pid==0) {
    // Child process, run the script
    exit(execv(path, args));
  }
  else {
    // Parent process
    waitpid(pid, &status, 0);
    if (WEXITSTATUS(status) == 0) {
      // Script executed correctly
      printf("[DEBUG] Script ran successfully\n");
    }
    else {
      // Some error
      printf("[DEBUG] Error in running script\n");
    }
  }
}


/**
 * Function to run the tunnel
 */
void run_tunnel(char *dest, int server)
{
  struct icmp_packet packet;
  int tun_fd, sock_fd;

  fd_set fs;

  tun_fd = tun_alloc("tun0", IFF_TUN | IFF_NO_PI);

  printf("[DEBUG] Starting tunnel - Dest: %s, Server: %d\n", dest, server);
  printf("[DEBUG] Opening ICMP socket\n");
  sock_fd = open_icmp_socket();

  if (server) {
    printf("[DEBUG] Binding ICMP socket\n");
    bind_icmp_socket(sock_fd);
  }

  configure_network(server);

  while (1) {
    FD_ZERO(&fs);
    FD_SET(tun_fd, &fs);
    FD_SET(sock_fd, &fs);

    select(tun_fd>sock_fd?tun_fd+1:sock_fd+1, &fs, NULL, NULL, NULL);

    if (FD_ISSET(tun_fd, &fs)) {
      printf("[DEBUG] Data needs to be readed from tun device\n");
      // Reading data from tun device and sending ICMP packet

      printf("[DEBUG] Preparing ICMP packet to be sent\n");
      // Preparing ICMP packet to be sent
      memset(&packet, 0, sizeof(struct icmp_packet));
      printf("[DEBUG] Destination address: %s\n", dest);

      if (sizeof(DEFAULT_ROUTE) > sizeof(packet.src_addr)){
        perror("Lack of space: size of DEFAULT_ROUTE > size of src_addr\n");
        close(tun_fd);
        close(sock_fd);
        exit(EXIT_FAILURE);
      }
      strncpy(packet.src_addr, DEFAULT_ROUTE, strlen(DEFAULT_ROUTE) + 1);

      if ((strlen(dest) + 1) > sizeof(packet.dest_addr)){
        perror("Lack of space for copy size of DEFAULT_ROUTE > size of dest_addr\n");
        close(sock_fd);
        exit(EXIT_FAILURE);
      }
      strncpy(packet.dest_addr, dest, strlen(dest) + 1);

      if(server) {
        set_reply_type(&packet);
      }
      else {
        set_echo_type(&packet);
      }
      packet.payload = calloc(MTU, sizeof(uint8_t));
      if (packet.payload == NULL){
        perror("No memory available\n");
        exit(EXIT_FAILURE);
      }

      packet.payload_size  = tun_read(tun_fd, packet.payload, MTU);
      if(packet.payload_size  == -1) {
        perror("Error while reading from tun device\n");
        exit(EXIT_FAILURE);
      }

      printf("[DEBUG] Sending ICMP packet with payload_size: %d, payload: %s\n", packet.payload_size, packet.payload);
      // Sending ICMP packet
      send_icmp_packet(sock_fd, &packet);

      free(packet.payload);
    }

    if (FD_ISSET(sock_fd, &fs)) {
      printf("[DEBUG] Received ICMP packet\n");
      // Reading data from remote socket and sending to tun device

      // Getting ICMP packet
      memset(&packet, 0, sizeof(struct icmp_packet));
      receive_icmp_packet(sock_fd, &packet);

      printf("[DEBUG] Read ICMP packet with src: %s, dest: %s, payload_size: %d, payload: %s\n", packet.src_addr, packet.dest_addr, packet.payload_size, packet.payload);
      // Writing out to tun device
      tun_write(tun_fd, packet.payload, packet.payload_size);

      printf("[DEBUG] Src address being copied: %s\n", packet.src_addr);
      strncpy(dest, packet.src_addr, strlen(packet.src_addr) + 1);
      free(packet.payload);
    }
  }

}
