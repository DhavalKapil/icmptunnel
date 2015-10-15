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
#include <errno.h>

/**
 *	function to send file
 */
void send_file(char *filename,char *src_ip , char *dest_ip)
{
	int fd;
	char *payload=(char *)malloc(MAX_PAYLOAD);
	if((fd = open( filename,  O_RDONLY )) == -1)
	{
		printf("Error in opening file : %s\n", strerror(errno));
		exit(-1);
	}
	//printf("pppp\n");
	struct icmp_packet packet;

	strcpy(packet.src_addr, src_ip);
  strcpy(packet.dest_addr, dest_ip);

	while(1)
	{

		int count = read(fd , payload , MAX_PAYLOAD);
  	
  		packet.payload_size = count+1; 
  		packet.payload = (char *)malloc(packet.payload_size+1);
  		printf("packet sent\n");
		if(count == 0 )
		{
			specify_packet(&packet , 0);
			send_packet(&packet);
			break;
		}

		specify_packet(&packet , 1);
		strcat(packet.payload, payload);
		send_packet(&packet);
	}
	close(fd);
}
/**
 *  function to specify packet type(data packet or connection-end packet)
 */
void specify_packet(struct icmp_packet *packet , int id)
{
	if(id ==0 )
		strcpy(packet->payload,"0");
	else
		strcpy(packet->payload,"1");

}

/**
 *	function to receive file
 */
void receive_file(char *filename)
{
	int fd;
	char curr;
	struct icmp_packet packet;
		
	if((fd = open(filename , O_WRONLY)) == -1)
	{
		printf("Error in opening file : %s\n", strerror(errno));
		exit(-1);
	}

	while(1)
	{
		//printf("where");
		
		receive_packet(&packet);
		//printf("are");
		curr = (packet.payload)[0];
		//printf("you");
		if(curr == 48)
			break;
		//if(curr == 49)
		printf("%s\n",packet.payload+1 );
		write(fd , &((packet.payload)[1]) , (packet.payload_size-1));
	}
	close(fd);
}

