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

//define port
#define PORT 8800

//define primitives
#define START_IDENTIFIER  0XFFFF
#define END_IDENTIFIER  0XFFFF
#define CLIENT_ID  0XFF
#define LENGTH  0XFF

//define packet Types
#define  DATA 0XFFF1
#define  ACK  0XFFF2
#define  REJECT 0XFFF3

//define sub codes
#define REJECT_OUT_SEQUENCE 0XFFF4
#define REJECT_lENGTH_MISMATCH 0XFFF5
#define REJECT_DATA_MISSING  0XFFF6
#define REJECT_DUPLICATE 0XFFF7

//structure of Data Packet
struct DataPacket{
	uint16_t StartPacketId;
	uint8_t ClientId;
	uint16_t Data;
	uint8_t SegmentNo;
	uint8_t Length;
	unsigned int Payload;
	uint16_t EndPacketId;
};

//Structure of ACK packet
struct ACKPacket{
	uint16_t StartPacketId;
	uint8_t ClientId;
	uint16_t Ack;
	uint8_t ReceivedSegmentNo;
	uint16_t EndPacketId;
};

//structure of Reject Pakcet
struct RejectPacket{
	uint16_t StartPacketId;
	uint8_t ClientId;
	uint16_t Reject;
	uint16_t Reject_sub_code;
	uint8_t ReceivedSegmentNo;
	uint16_t EndPacketId;
};


int main()
{

        char *ip = "127.0.0.1";

        int sockfd;
        struct sockaddr_in addr;
        char buffer[1024];
        socklen_t addr_size;

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = inet_addr(ip);

	
	bzero(buffer, 1024);
	strcpy(buffer, "Hello, world!");
	sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)&addr, sizeof(addr));
	printf("[+]Data send:%s \n",buffer );

	bzero(buffer, 1024);
	addr_size = sizeof(addr);
	recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&addr, &addr_size);
	printf("[+]Data recv: %s\n", buffer);	

	return 0;
}
