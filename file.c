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
	char payload[MAX_PAYLOAD];
	if((fd = open( filename,  O_RDONLY )) == -1)
	{
		printf("Error in opening file : %s\n", strerror(errno));
		exit(-1);
	}

	while(1)
	{
		payload[0]='\0';
		int count = read(fd , payload , MAX_PAYLOAD);
		struct icmp_packet packet;
		strcpy(packet.src_addr, src_ip);
  		strcpy(packet.dest_addr, dest_ip);
  		packet.payload = payload;
  		packet.payload_size = count+1;
  		printf("packet sent");
		if(count == 0 )
		{
			specify_packet(&packet , 0);
			send_packet(&packet);
			break;
		}
		specify_packet(&packet , 1);
		send_packet(&packet);
	}
	close(fd);
}
/**
 *  function to specify packet type(data packet or connection-end packet)
 */
void specify_packet(struct icmp_packet *packet , int id)
{
	char *spec = (id == 0)?"0":"1";
	char *temp = (char *)malloc(MAX_PAYLOAD+5);
	temp = "";
	strcat(temp,spec);
	strcat(temp,packet->payload);
	packet->payload = temp;
}
 
/**
 *	function to receive file
 */
void receive_file(char *filename)
{
	int fd;
	char curr;
	if((fd = open(filename , O_WRONLY)) == -1)
	{
		printf("Error in opening file : %s\n", strerror(errno));
		exit(-1);
	}

	while(1)
	{
		//printf("where");
		struct icmp_packet packet;
		receive_packet(&packet);
		//printf("are");
		curr = (packet.payload)[0];
		if(curr == 48)
			break;
		//if(curr == 49)
		write(fd , ((packet.payload)+1) , (packet.payload_size-1));
	}
	close(fd);
}
