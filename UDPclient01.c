/*
COEN 233 Computer Networks
Program assignment 1
Name: Li Huang
Student ID: W1641460
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "packet.h"

//define port
#define PORT 8800


int main()
{

	char *ip = "127.0.0.1";

        int sockfd;
        struct sockaddr_in addr;
        
	// char buffer[1024];
	DataPacket packet;
	uint8_t *buffer = (uint8_t *)(&packet);
	
        socklen_t addr_size;
	
	//create socket
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	// apply address
        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = inet_addr(ip);



	bzero(buffer, sizeof(DataPacket));
	strcpy(buffer, "Hello, world!");
	sendto(sockfd, buffer, sizeof(DataPacket), 0, (struct sockaddr*)&addr, sizeof(addr));
	printf("[+]Data send:%s \n",buffer );

	bzero(buffer, sizeof(DataPacket));
	addr_size = sizeof(addr);
	recvfrom(sockfd, buffer, sizeof(DataPacket), 0, (struct sockaddr*)&addr, &addr_size);
	printf("[+]Data recv: %s\n", buffer);	

	return 0;
}
