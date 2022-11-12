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

//define prot
#define PORT 8800

//function to print packets contents
void show(struct DataPacket dtp){
	printf("\nStart of Packet id:%x\n", dtp.StartPacketId);
	printf("\nClient ID:%x\n", dtp.ClientId);
	printf("\nDATA:%x\n",dtp.Data);
	printf("\nSegment No:%x\n", dtp.SegmentNo);
	printf("\nLength:%x\n", dtp.SegmentNo);
	printf("\nPayload:%x\n", dtp.Payload);
	printf("\nEnd of Packet id:%x\n", dtp.EndPacketId);

}



int main()
{

        char *ip = "127.0.0.1";

        int sockfd, b;
        struct sockaddr_in server_addr, client_addr;
        
	//char buffer[1024];
        DataPacket packet;
        uint8_t *buffer = (uint8_t *)(&packet);
	
        socklen_t addr_size;

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0){
                perror("socket error!");
                exit(1);
        }

        memset(&server_addr, '\0', sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = inet_addr(ip);
	
	b = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if(b < 0){
                perror("bind error!");
                exit(1);
        }

	bzero(buffer, sizeof(DataPacket));
	addr_size = sizeof(client_addr);
	recvfrom(sockfd, buffer, sizeof(DataPacket), 0,(struct sockaddr*)&client_addr, &addr_size);
	printf("Data recv: %s \n", buffer);

	DataPacket *dp = (DataPacket *)buffer;
	show(*dp);
        
	bzero(buffer, sizeof(DataPacket));
	strcpy(buffer, "Welcome to the UDP server!");
	sendto(sockfd, buffer, sizeof(DataPacket), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
	printf("Data send: %s\n", buffer);

	return 0;
}
