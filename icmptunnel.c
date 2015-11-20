/**
 * icmp_tunnel.c
 */

#include "tunnel.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  char ip_addr[100] = {0,};
  if ((strlen(argv[2]) + 1) > sizeof(ip_addr)){
      printf("Bad ip argument\n");
      exit(EXIT_FAILURE);
  }
  memcpy(ip_addr, argv[2], strlen(argv[2]) + 1);
  run_tunnel(ip_addr, !strcmp(argv[1], "-s"));
  return 0;
}
