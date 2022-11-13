#ifndef packet_h
#define packet_h


//define pClientIdrimitives
#define START_IDENTIFIER  0XFFFF
#define END_IDENTIFIER  0XFFFF

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
typedef struct DataPacket{
	uint16_t StartPacketId;
	uint8_t ClientId;
	uint16_t Data;
	uint8_t SegmentNo;
	uint8_t Length;
	uint8_t Payload[255];
	uint16_t EndPacketId;
}DataPacket;

//Structure of ACK packet
typedef struct ACKPacket{
	uint16_t StartPacketId;
	uint8_t ClientId;
	uint16_t Ack;
	uint8_t ReceivedSegmentNo;
	uint16_t EndPacketId;
}ACKPacket;

//structure of Reject Pakcet
typedef struct RejectPacket{
	uint16_t StartPacketId;
	uint8_t ClientId;
	uint16_t Reject;
	uint16_t Reject_sub_code;
	uint8_t ReceivedSegmentNo;
	uint16_t EndPacketId;
}RejectPacket;


#endif
