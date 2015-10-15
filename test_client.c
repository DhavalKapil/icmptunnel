#include "file.h"
#include <string.h>

int main()
{
  char *filename, *src_ip, *dest_ip;
  
  initialize_client();

  //send_packet(&packet);
  filename = (char *)malloc(100);
  src_ip = (char *)malloc(100);
  dest_ip = (char *)malloc(100);
  filename = "./send";
  src_ip = "127.0.0.1";
  dest_ip = "127.0.0.1";
  send_file(filename,src_ip,dest_ip);

  close_socket();
}