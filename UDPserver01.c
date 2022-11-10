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
#include <unistd.h>
//define port
#define PORT 22

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


int main(int argc, char **argv)
{
	struct DataPacket datapacket;
	struct ACKPacket Ackpacket;
	struct RejectPacket rejectpacket;

	int sockfd;
	struct sockaddr_in server_addr;
	socklen_t addr_size;
	char *ip = "127.0.0.1";

	//create socket
	sockfd = socket(AF_INET, SOCK_DGRAM,0);
	bzero(&server_addr, sizeof(server_addr));

	//assign address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(ip);
	
	//binding socket
	bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));




	return 0;
}
