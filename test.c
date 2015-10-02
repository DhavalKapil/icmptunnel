#include "icmp.h"
#include <string.h>

int main()
{
	struct icmp_packet packet;

	packet.src_addr = "127.0.0.1";
	packet.dest_addr = "127.0.0.1";
	packet.payload = "ZZZZZZ";
	packet.payload_size = strlen(packet.payload);

	initialize_client();

	send_packet(&packet);

	close_socket();
}