/**
 *  tunnel.h
 */

#ifndef tunnel_gaurd
#define tunnel_gaurd

/**
 * Function to allocate a tunnel
 */
int tun_alloc(char *dev, int flags);

/**
 * Function to run the tunnel
 */
void run_tunnel(int server);

#endif