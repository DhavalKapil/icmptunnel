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
 * Function to read from a tunnel
 */
int tun_read(int tun_fd, char *buffer, int length);

/**
 * Function to write to a tunnel
 */
int tun_write(int tun_fd, char *buffer, int length);

/**
 * Function to allocate a tunnel
 */
int tun_alloc(char *dev, int flags);


/**
 * Function to run the tunnel
 */
void run_tunnel(int tun_fd, char *dest, int server);

#endif