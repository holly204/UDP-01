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
	unit16_t StartPacketId;
	unit8_t ClientId;
	unit16_t Data;
	unit8_t SegmentNo;
	unit8_t Length;
	unsigned int Payload;
	unit16_t EndPacketId;	
}

//Structure of ACK packet
struct ACKPacket{
	unit16_t StartPacketId;
	unit8_t ClientId;
	unit16_t ACK;
	unit8_t ReceivedSegmentNo;
	unit16_t EndPacketId;
}
//structure of Reject Pakcet
struct RejectPacket{
	unit16_t StartPacketId;
	unit8_t ClientId;
	unit16_t Reject;
	unit16_t Reject_sub_code;
	unit8_t ReceivedSegmentNo;
	unit16 EndPacketId;
}


int main(int argc, char **argv)
{



	return 0;
}
