/**
 * icmp_tunnel.c
 */

#include "tunnel.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ARG_SERVER_MODE "-s"
#define ARG_CLIENT_MODE "-c"

void usage()
{
  printf("Wrong argument\n");
  fprintf(stdout, "usage: icmptunnel [-s serverip] | [-c clientip]\n");
}

int main(int argc, char *argv[])
{
  char ip_addr[100] = {0,};
  if ((argc < 3) || ((strlen(argv[2]) + 1) > sizeof(ip_addr))) {
    usage();
    exit(EXIT_FAILURE);
  }
  memcpy(ip_addr, argv[2], strlen(argv[2]) + 1);

  if (strncmp(argv[1], ARG_SERVER_MODE, strlen(argv[1])) == 0) {
    run_tunnel(ip_addr, 1);
  }
  else if (strncmp(argv[1], ARG_CLIENT_MODE, strlen(argv[1])) == 0) {
    run_tunnel(ip_addr, 0);
  }
  else {
    usage();
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
