#include "icmp.h"

#include <stdio.h>
#include <string.h>
 
 
int main()
{
  struct icmp_packet packet;
 
  open_icmp_socket();
  bind_icmp_socket();

  printf("server initialized\n");
  while(1)
  {
    receive_icmp_packet(&packet);
    printf("%s\n", packet.payload);
  }
 
  close_icmp_socket();
}