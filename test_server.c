#include <stdio.h>
#include "icmp_server.h"
#include <string.h>


int main()
{
	struct icmp_packet packet;

	initialize_server();
	printf("server initialized\n");

	receive_packet(&packet);
	printf("packet received\n");

	printf("%s : %s : %s : %d\n",packet.src_addr, packet.dest_addr, packet.payload , packet.payload_size);

	close_socket();
}