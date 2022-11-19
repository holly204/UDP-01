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
#include <errno.h>
#include "packet.h"

//define prot
#define PORT 8800

//define a function for show datapacket
void show(struct DataPacket dtp);

ACKPacket generate_recv(struct DataPacket dp);
RejectPacket generate_rej(struct DataPacket dp, int Rej_sub_code);
void show_ack(struct ACKPacket ap);
void show_rej(struct RejectPacket rp);
int receive_packet(int sockfd,struct sockaddr_in*client_addr,socklen_t addr_size);

int main()
{

        char *ip = "127.0.0.1";

        int sockfd, b;
        struct sockaddr_in server_addr, client_addr;
        
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

        receive_packet(sockfd, &client_addr, addr_size);

	return 0;
}

ACKPacket generate_recv(struct DataPacket dp){
	struct ACKPacket rp;
	rp.StartPacketId = dp.StartPacketId;
	rp.ClientId = dp.ClientId;
	rp.Ack = ACK;
	rp.ReceivedSegmentNo = dp.SegmentNo;
	rp.EndPacketId = dp.EndPacketId;
	return rp;
}
RejectPacket generate_rej(struct DataPacket dp, int Rej_sub_code){
        struct RejectPacket jp;
        jp.StartPacketId = START_IDENTIFIER;
        jp.ClientId = dp.ClientId;
        jp.Reject = REJECT;
	if (Rej_sub_code == 1){
		jp.Reject_sub_code = REJECT_OUT_SEQUENCE;
	}else if(Rej_sub_code == 2){
		jp.Reject_sub_code = REJECT_lENGTH_MISMATCH;
	}else if(Rej_sub_code == 3){
		jp.Reject_sub_code = REJECT_DATA_MISSING;
	}else if(Rej_sub_code == 4){
		jp.Reject_sub_code = REJECT_DUPLICATE;
	}else{
		jp.Reject_sub_code = 0;
	}
        jp.ReceivedSegmentNo = dp.SegmentNo;
        jp.EndPacketId = END_IDENTIFIER;
        return jp;
}


int receive_packet(int sockfd,struct sockaddr_in *client_addr, socklen_t addr_size) {
        // 1. call recvfrom to receive packet from client
        // 2. check packet, if packet is wrong, print error
        // 3. if packet is correct, send ack to client, otherwise, send reject.
        // 4. replace last packet segno to be current packet seqno, if segno = last seg + 1
        int rev = 1;
	int last_seg = 0;
	int response_type = 0;//0:ack, 1:out of sequence, 2:lenth mismatching, 3:End of packet missing, 4: duplicate
	DataPacket *dp = malloc(sizeof(DataPacket));
	while(rev>0){
		addr_size = sizeof(client_addr);
		uint8_t *buffer = (uint8_t *)dp;
        	rev = recvfrom(sockfd, buffer, sizeof(DataPacket), 0,(struct sockaddr*)&client_addr, &addr_size);
		printf("received %d bytes\n", rev);
		show(*dp);
		if(dp->ClientId==0){
			printf("Ignore client 0\n");
			break;
		}
		if (dp->SegmentNo != last_seg + 1){
			if(dp->SegmentNo == last_seg){
				response_type = 4; //duplicate
			}
			else{
				 response_type = 1; //out of sequence
			}
		} else {
			last_seg = dp->SegmentNo;
			printf("Segment number updated to %d\n", last_seg);
		}

		//check missing EndPacketId
		if (dp->EndPacketId != END_IDENTIFIER){
			response_type = 3;
		}

		//check payload length
		if(dp->Length != sizeof(dp->Payload)){
			response_type = 2;
		}
		if (response_type == 0){
			ACKPacket *ap = malloc(sizeof(ACKPacket));
			*ap = generate_recv(*dp);
			//show_ack(*ap);
			//buffer = (uint8_t *)ap;

			// Must use the addr_size from the previous recvfrom to specify addr length
			int send_ack = sendto(sockfd, ap, sizeof(ACKPacket), 0, (struct sockaddr*)&client_addr, addr_size);
			printf("ACK send ack %d bytes\n", send_ack);
			show_ack(*ap);
			//printf("ACK send ack %d bytes, errno=%d\n", send_ack, errno);
		}
		else{
			RejectPacket *rjp = malloc(sizeof(RejectPacket));
			*rjp = generate_rej(*dp, response_type);
			//show_ack(*ap);
			//buffer = (uint8_t *)rjp;
			//uint8_t ack_or_reject[sizeof(ACKPacket) + sizeof(RejectPacket)] = {0};

			// Must use the addr_size from the previous recvfrom to specify addr length
			int send_rjk = sendto(sockfd, rjp, sizeof(RejectPacket), 0, (struct sockaddr*)&client_addr, addr_size);
			//printf("Rej send  %d bytes, errno=%d\n", send_rjk, errno);
			printf("Send Reject");
		}
	}	
       	return rev;
}

void show(struct DataPacket dtp){
        printf("\nStart of Packet id:%x ", dtp.StartPacketId);
        printf("\nClient ID:%x  ", dtp.ClientId);
        printf("\nDATA:%x ",dtp.Data);
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
        printf("\nEnd of Packet id:%x \n", rp.EndPacketId);
}

