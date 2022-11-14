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
//define COUNT 5
#define COUNT 5
//define a function for get datapacket
DataPacket getDatapacket(int clientid, int n);
//define a function for show datapacket
void show(struct DataPacket dtp);
// define send_packet function
int send_packet(int sockfd, struct sockaddr_in *addr, DataPacket *dp, socklen_t addr_size); 

int main()
{

	char *ip = "127.0.0.1";

        int sockfd;
        struct sockaddr_in addr;
        
	// char buffer[1024];
	
        socklen_t addr_size;
	
	//create socket
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	// apply address
        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = inet_addr(ip);


        for (int i=1; i<= COUNT; i++ ){
                DataPacket packet = getDatapacket(11,i);
                send_packet(sockfd, &addr, &packet, addr_size);
        }

/*
	bzero(buffer, sizeof(DataPacket));
	strcpy(buffer, "Hello, world!");
	//sendto(sockfd, buffer, sizeof(DataPacket), 0, (struct sockaddr*)&addr, sizeof(addr));
	printf("[+]Data send:%s \n",buffer );

	bzero(buffer, sizeof(DataPacket));
	addr_size = sizeof(addr);
	recvfrom(sockfd, buffer, sizeof(DataPacket), 0, (struct sockaddr*)&addr, &addr_size);
	printf("[+]Data recv: %s\n", buffer);
*/	

	return 0;
}

DataPacket getDatapacket(int clientid,int n) {


        struct DataPacket dp;

        dp.StartPacketId = START_IDENTIFIER;
        dp.ClientId = clientid;
        dp.Data = DATA;
        dp.SegmentNo = n;
	strcpy(dp.Payload,"The packet is send by client");
        dp.Length = strlen(dp.Payload);
        dp.EndPacketId = END_IDENTIFIER;

        return dp;
}
int send_packet(int sockfd, struct sockaddr_in *addr, DataPacket *dp, socklen_t addr_size) {
        // 1. Call sendto to send packet to server.
        // 2. Call recvfrom to receive ack / reject from server.
        // 3. If timeout, start timer, go back to 2 if not running out of retries
        // 4. If get ack / reject, check packet, return 0 for sucess, or error code for failures
        // 5. Print error (ack, reject wrong / final timeout), return final error cod
	uint8_t *buffer = (uint8_t *)(dp);
        int ret = sendto(sockfd, buffer, sizeof(*dp),0, (struct sockaddr *)addr, sizeof(*addr));
	printf("Packet send %d bytes\n", ret);

	//receive ack packet
	ACKPacket *ap = malloc(sizeof(ACKPacket));
        printf("begin buffer");

	buffer = (uint8_t *)(ap);	
	addr_size = sizeof(addr);
	printf("begin receive ack");
	int rev_ack = recvfrom(sockfd, buffer, sizeof(ACKPacket), 0, (struct sockaddr*)&addr, &addr_size);
	printf("ACK received %d bytes\n", rev_ack);
	for(int i = 0; i < sizeof(*ap); i++) {
                printf("%x ", buffer[i]);
        }
        printf("\n");

	printf("Ack received");
	return ret;
}
void show(struct DataPacket dtp){
        printf("\nStart of Packet id:%x\n", dtp.StartPacketId);
        printf("\nClient ID:%x\n", dtp.ClientId);
        printf("\nDATA:%x\n",dtp.Data);
        printf("\nSegment No:%x\n", dtp.SegmentNo);
        printf("\nLength:%x\n", dtp.SegmentNo);
        printf("\nPayload:%x\n", dtp.Payload);
        printf("\nEnd of Packet id:%x\n", dtp.EndPacketId);

}

