#include "icmp.h"

#include <stdio.h>
#include <string.h>
 
 
int main()
{
  struct icmp_packet packet;
  int sock_fd;
 
  sock_fd = open_icmp_socket();
  bind_icmp_socket(sock_fd);

  printf("server initialized\n");
  while(1)
  {
    receive_icmp_packet(sock_fd, &packet);
    printf("%s\n", packet.src_addr);
    printf("%s\n", packet.dest_addr);
    printf("%d\n", packet.type);
    printf("%s\n", packet.payload);
  }
 
  close_icmp_socket(sock_fd);
}