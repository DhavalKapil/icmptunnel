/**
 * icmp_tunnel.c
 */

#include "tunnel.h"

#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  run_tunnel(argv[2], !strcmp(argv[1], "-s"));
  return EXIT_SUCCESS;
}
