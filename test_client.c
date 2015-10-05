#include "icmp_client.h"
#include <string.h>

int main()
{
  struct icmp_packet packet;
  char *src_ip;
  char *dest_ip;

  src_ip = "127.0.0.2";
  dest_ip = "127.0.0.1";

  strcpy(packet.src_addr, src_ip);
  strcpy(packet.dest_addr, dest_ip);
  packet.payload = "ZZZZZZ";
  packet.payload_size = strlen(packet.payload);

  initialize_client();

  send_packet(&packet);

  close_socket();
}