/*
COEN 233 Computer Networks Program assignment 1
Client using customized protocol on top of UDP protocol for sending information to the server.
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
#include <unistd.h>
#include "packet.h"

//define prot
#define PORT 8800

//define function
ACKPacket generate_recv(struct DataPacket dp);
RejectPacket generate_rej(struct DataPacket dp, int Rej_sub_code);
void show(struct DataPacket dtp);
void show_ack(struct ACKPacket ap);
void show_rej(struct RejectPacket rp);
int server_loop(int sockfd,struct sockaddr_in*client_addr,socklen_t addr_size);

int main()
{

        char *ip = "127.0.0.1";
        int sockfd, b;
        struct sockaddr_in server_addr, client_addr;       
        socklen_t addr_size;
        //create socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0){
                perror("socket error!");
                exit(1);
        }

        memset(&server_addr, '\0', sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = inet_addr(ip);
	//bind
	b = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if(b < 0){
                perror("bind error!");
                exit(1);
        
	}
	//receive and send back function
        server_loop(sockfd, &client_addr, addr_size);
	close(sockfd);
	return 0;
}

//generate ACKpacket from DataPacket
ACKPacket generate_recv(struct DataPacket dp){
	struct ACKPacket rp;
	rp.StartPacketId = dp.StartPacketId;
	rp.ClientId = dp.ClientId;
	rp.Ack = ACK;
	rp.ReceivedSegmentNo = dp.SegmentNo;
	rp.EndPacketId = dp.EndPacketId;
	return rp;
}
//generate RejectPacket from DataPacket
RejectPacket generate_rej(struct DataPacket dp, int Rej_sub_code){
        struct RejectPacket jp;
        jp.StartPacketId = dp.StartPacketId;
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
        jp.EndPacketId = dp.EndPacketId;
        return jp;
}

//receive and send back function
int server_loop(int sockfd,struct sockaddr_in *client_addr, socklen_t addr_size) {
        // 1. call recvfrom to receive packet from client
        // 2. check packet, if packet is wrong, print error
        // 3. if packet is correct, send ack to client, otherwise, send reject.
        // 4. replace last packet segno to be current packet seqno, if segno = last seg + 1
        int rev = 1;
	int last_seg = 0;
	int response_type = 0;//0:ack, 1:out of sequence, 2:lenth mismatching, 3:End of packet missing, 4: duplicate
	uint8_t buffer[sizeof(DataPacket) + sizeof(ACKPacket) + sizeof(RejectPacket)] = {0};
	DataPacket *dp = (DataPacket *)buffer;
	while(rev>0){
		//receive Datapacket
		addr_size = sizeof(client_addr);
        	rev = recvfrom(sockfd, buffer, sizeof(DataPacket), 0,(struct sockaddr*)&client_addr, &addr_size);
		response_type = 0;
		show(*dp);

		if(dp->ClientId==0){
			//Just for test packet 3 times without respond
			printf("Ignore client 0\n");
			continue;
		}
		if (dp->SegmentNo != last_seg + 1){
			if(dp->SegmentNo == last_seg){
				response_type = 4; //duplicate
			}
			else{
				 response_type = 1; //out of sequence
			}
		} 
		printf("EndPacketId: %x", dp->EndPacketId);
		//check missing EndPacketId
		if (dp->EndPacketId != END_IDENTIFIER){
			response_type = 3;
		}

		//check payload length
		if(dp->Length != strlen(dp->Payload)){
			response_type = 2;
		}
		if (response_type == 0){
			//Update last segmentNo when respond ACK
			last_seg = dp->SegmentNo;
			printf("Segment number updated to %d\n", last_seg);
			ACKPacket *ap = (ACKPacket *)buffer;
			*ap = generate_recv(*dp);

			// Must use the addr_size from the previous recvfrom to specify addr length
			int send_ack = sendto(sockfd, ap, sizeof(ACKPacket), 0, (struct sockaddr*)&client_addr, addr_size);
			show_ack(*ap);
		}
		else{
			RejectPacket *rjp = (RejectPacket *)buffer;
			*rjp = generate_rej(*dp, response_type);

			// Must use the addr_size from the previous recvfrom to specify addr length
			int send_rjk = sendto(sockfd, rjp, sizeof(RejectPacket), 0, (struct sockaddr*)&client_addr, addr_size);
			show_rej(*rjp);
		}
	}	
       	return rev;
}

void show(struct DataPacket dtp){
        printf("*****Received Datapacket from clinet***************\n");
	printf("Start of Packet id:%x \n", dtp.StartPacketId);
        printf("Client ID:%d  \n", dtp.ClientId);
        printf("DATA:%x \n",dtp.Data);
        printf("Segment No:%d \n", dtp.SegmentNo);
        printf("Length:%x \n", dtp.Length);
        printf("Payload:%s \n", dtp.Payload);
        printf("End of Packet id:%x \n", dtp.EndPacketId);

}
void show_ack(struct ACKPacket ap){
	printf("********ACK Packet was send back to client***********\n");
        printf("Start of Packet id:%x \n", ap.StartPacketId);
	printf("Client id:%d \n", ap.ClientId);
        printf("Ack:%x \n", ap.Ack);
        printf("received no:%d \n", ap.ReceivedSegmentNo);
	printf("End of Packet id:%x \n", ap.EndPacketId);
}

void show_rej(struct RejectPacket rp){
	printf("********Reject Packet was send back to client***********\n");
        printf("Start of Packet id:%x \n", rp.StartPacketId);
        printf("Client id:%d \n", rp.ClientId);
        printf("Rej:%x \n", rp.Reject);
        printf("reject sub no:%x \n", rp.Reject_sub_code);
	printf("received no:%d \n", rp.ReceivedSegmentNo);
        printf("End of Packet id:%x \n", rp.EndPacketId);
}

