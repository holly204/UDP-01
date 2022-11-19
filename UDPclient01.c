/*
COEN 233 Computer Networks
Program assignment 1
Name: Li Huang
Student ID: W1641460
*/

#include <errno.h>
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
void show_ack(struct ACKPacket ap);
void show_rej(struct RejectPacket rp);
// define send_packet function
int send_packet(int sockfd, struct sockaddr_in *addr, DataPacket *dp, socklen_t addr_size); 

int main()
{

	char *ip = "127.0.0.1";
	int clientid = 11;
        int sockfd;
        struct sockaddr_in addr;
        
        socklen_t addr_size;
	
	//create socket
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	//set ack_timer
	struct timeval ack_timer;
	// Timeout Value as required
	ack_timer.tv_sec = 3;
	ack_timer.tv_usec = 0;

	// verifying for connection to the server
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ack_timer,sizeof(struct timeval));	
	// apply address
        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = inet_addr(ip);
	
	DataPacket packet;
	// send 5 correct packets
        for (int i=1; i<= COUNT; i++ ){
               packet = getDatapacket(clientid,i);
               send_packet(sockfd, &addr, &packet, addr_size);
        }

	//send 1 correct and 4 incorrect packets
	//1 correct packet
	//packet = getDatapacket(1,1);
	//send_packet(sockfd, &addr, &packet, addr_size);
	
	packet = getDatapacket(clientid,5);
	send_packet(sockfd, &addr, &packet, addr_size);	
	
	//2 out of sequence
	//packet = getDatapacket(11,8);
	//send_packet(sockfd, &addr, &packet, addr_size);

	//3 length mismatch
	//packet = getDatapacket(11,1);
	//packet.Length = 3;
	//send_packet(sockfd, &addr, &packet, addr_size);
	
	//4 End of packet missing
	//packet = getDatapacket(11,1);
	//packet.EndPacketId = 0;
	//send_packet(sockfd, &addr, &packet, addr_size);
	
	//5 Duplicate packet	
	//packet = getDatapacket(11,10);
	//send_packet(sockfd, &addr, &packet, addr_size);

	return 0;
}

DataPacket getDatapacket(int clientid,int n) {

        struct DataPacket dp;

        dp.StartPacketId = START_IDENTIFIER;
        dp.ClientId = clientid;
        dp.Data = DATA;
        dp.SegmentNo = n;
	sprintf(dp.Payload, "This packet is sent by:%d and SegmentNo is %d.", clientid, n);
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
	int ret =1;
	int rev_pack = 0;
	
	for (int i = 0; i < 3; i++) {
		uint8_t *buffer;
		buffer = (uint8_t *)(dp);
        	ret = sendto(sockfd, buffer, sizeof(*dp),0, (struct sockaddr *)addr, sizeof(*addr));
		printf("Packet send %d bytes\n", ret);
		show(*dp);
		//receive packet
		printf("begin packet receive\n");
		addr_size = sizeof(addr);	
		uint8_t *ack_or_reject[sizeof(RejectPacket)] = {0};
		//RejectPacket *rjp = malloc(sizeof(RejectPacket));
		rev_pack = recvfrom(sockfd, ack_or_reject, sizeof(ack_or_reject), 0, (struct sockaddr*)&addr, &addr_size);
		printf("ACK received %d bytes\n", rev_pack);
		if(rev_pack == sizeof(ACKPacket)){
			ACKPacket *ack;
			ack = (ACKPacket*)ack_or_reject;
			show_ack(*ack);
			printf("Clientid: %d ,Segment No %d ACK was send\n", ack->ClientId,ack->ReceivedSegmentNo);
			return 0;
		}else if(rev_pack == sizeof(RejectPacket)){
			RejectPacket *rjp;
			rjp = (RejectPacket *)ack_or_reject;
			show_rej(*rjp);
			if(rjp->Reject_sub_code == REJECT_OUT_SEQUENCE){
				printf("Clientid: %d ,Segment No %d was Reject due to Out of sequence\n", rjp->ClientId,rjp->ReceivedSegmentNo);
			}else if(rjp->Reject_sub_code == REJECT_lENGTH_MISMATCH){
				printf("Clientid: %d ,Segment No %d was Reject due to length mismatch\n", rjp->ClientId,rjp->ReceivedSegmentNo);
			}else if(rjp->Reject_sub_code == REJECT_DATA_MISSING){
				printf("Clientid: %d ,Segment No %d was Reject due to End of packet missing\n", rjp->ClientId,rjp->ReceivedSegmentNo);
			}else if(rjp->Reject_sub_code == REJECT_DUPLICATE){
				printf("Clientid: %d ,Segment No %d was Reject due to Duplicate packet\n", rjp->ClientId,rjp->ReceivedSegmentNo);
			}	
			return -rjp->Reject_sub_code;		
		}
		else{
			printf("[ERROR] Receive error, rev_pack=%d, errno=%d\n", rev_pack, errno);
			continue;
		}
	}
	printf("Server does not respond\n");
	return -255;
}

void show(struct DataPacket dtp){
        printf("\nStart of Packet id:%x ", dtp.StartPacketId);
        printf("\nClient ID:%x ", dtp.ClientId);
        printf("\nDATA:%x",dtp.Data);
        printf("\nSegment No:%x ", dtp.SegmentNo);
        printf("\nLength:%x ", dtp.Length);
        printf("\nPayload:%s ", dtp.Payload);
        printf("\nEnd of Packet id:%x \n", dtp.EndPacketId);

}
void show_ack(struct ACKPacket ap){

        printf("\nStart of Packet id:%x ", ap.StartPacketId);
        printf("\nClient id:%x ", ap.ClientId);
        printf("\nAck:%x ", ap.Ack);
        printf("\nreceived no:%x ", ap.ReceivedSegmentNo);
        printf("\nEnd of Packet id:%x \n", ap.EndPacketId);
}

void show_rej(struct RejectPacket rp){

        printf("\nStart of Packet id:%x ", rp.StartPacketId);
        printf("\nClient id:%x ", rp.ClientId);
        printf("\nRej:%x ", rp.Reject);
        printf("\nreject sub no:%x ", rp.Reject_sub_code);
        printf("\nreceived no:%x ", rp.ReceivedSegmentNo);
        printf("\nEnd of Packet id:%x\n ", rp.EndPacketId);
}

