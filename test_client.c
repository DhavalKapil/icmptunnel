#include "icmp.h"
#include <string.h>

int main()
{
  struct icmp_packet packet;
  char *src_ip;
  char *dest_ip;
  int sock_fd;

  src_ip = "127.0.0.2";
  dest_ip = "127.0.0.1";

  strncpy(packet.src_addr, src_ip, strlen(src_ip) + 1);
  strncpy(packet.dest_addr, dest_ip, strlen(dest_ip) + 1);
  set_reply_type(&packet);
  packet.payload = "ZZZZZZ";
  packet.payload_size = strlen(packet.payload);

  sock_fd = open_icmp_socket();

  send_icmp_packet(sock_fd, &packet);

  close_icmp_socket(sock_fd);
}
