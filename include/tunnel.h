/**
 *  @file tunnel.h
 */

#ifndef tunnel_gaurd
#define tunnel_gaurd

#define SERVER_SCRIPT "server.sh"
#define CLIENT_SCRIPT "client.sh"

/**
 * @brief Function to allocate a tunnel
 */
int tun_alloc(char *dev, int flags);

/**
 * @brief Function to read from a tunnel
 */
int tun_read(int tun_fd, char *buffer, int length);

/**k
 * @brief Function to write to a tunnel
 */
int tun_write(int tun_fd, char *buffer, int length);

/**
 * @brief Function to run the tunnel
 */
void run_tunnel(char *dest, int server);

#endif
