/**
 *	file.h
 */

#include "icmp.h"
#include "icmp_client.h"
#include "icmp_server.h"
#include <stdint.h>

#ifndef file_guard
#define file_guard

#define MAX_PAYLOAD 1400

/**
 *  function to specify packet type(data packet or connection-end packet)
 */
void specify_packet(struct icmp_packet *packet , int id);

/**
 *	function to receive file
 */
void receive_file(char *filename);

/**
 *	function to send file
 */
void send_file(char *filename, char *src_ip, char *dest_ip);

#endif
