#include "icmp.h"
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
  set_reply_type(&packet);
  packet.payload = "ZZZZZZ";
  packet.payload_size = strlen(packet.payload);

  open_icmp_socket();

  send_icmp_packet(&packet);

  close_icmp_socket();
}