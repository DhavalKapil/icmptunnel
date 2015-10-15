#include "icmp_server.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char *filename;
  	struct icmp_packet packet;

  	initialize_server();
  	printf("server initialized\n");

  /*receive_packet(&packet);
  printf("packet received\n");
  printf("%s : %s : %s : %d\n",packet.src_addr, packet.dest_addr, packet.payload , packet.payload_size);
  */
  	filename = (char *)malloc(100);
	filename = "./recv";
	receive_file(filename);

  	close_socket();
}