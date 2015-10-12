/**
 *	file.c
 */

#include "file.h"
#include "icmp.h"
#include "icmp_client.h"
#include "icmp_server.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *	function to send file
 */
void send_file(char *filename,char *src_ip , char *dest_ip)
{
	int fd;
	char *payload;
	if((fd = open( filename,  O_RDONLY )) == -1)
		printf("error");
	payload =(char *)malloc(MAX_PAYLOAD); 
	while(1)
	{
		int count = read(fd , payload , MAX_PAYLOAD);
		struct icmp_packet packet;
		if(count == 0 )
			break;
		strcpy(packet.src_addr, src_ip);
  		strcpy(packet.dest_addr, dest_ip);
		packet.payload = payload;
		packet.payload_size = count;
		send_packet(&packet);
	}
	close(fd);
}

/**
 *	function to receive file
 */
void receive_file(char *filename)
{
	int fd;
	char *payload;
	if((fd = open(filename , O_WRONLY)) == -1)
		printf("error");
	payload = (char *) malloc(MAX_PAYLOAD);
	while(1)
	{
		struct icmp_packet packet;
		receive_packet(&packet);
		int count = write(fd , packet.payload , packet.payload_size);
		if(packet.payload_size < MAX_PAYLOAD)
			break;
	}
	close(fd);
}
