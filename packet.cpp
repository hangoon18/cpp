#include <iostream>
#include <memory>
#include "packet.h"
using namespace std;

//unsigned char InstructionPacket[MAXIMUM_TX + 10] = { 0, };	// instruction 패킷 array
//unsigned char StatusPacket[MAXIMUM_RX + 10] = { 0, };		// rx 패킷 array

Comm::Comm() {	

	memset(InstructionPacket, 0, MAXIMUM_TX + 10);		// instruction pakcet 연속범위로 바이트단위로 지정 160
	memset(StatusPacket,0,MAXIMUM_RX +10);				// rx packet 연속범위로 byte 단위로 지정 160
	RxPacketLength = 0;									// rx length init
	RxGetLength = 0;									// rx get length init
	CommStatus = COMM_RXSUCCESS;						// rx success status
	BusUsing = 0;										// bus status init
	
}// Comm constructor

int __stdcall Comm::com_initialize(int devIndex, int baudnum) {

	float baudrate;										
	baudrate = 2000000.0f / (float)(baudnum + 1);

	if (dxl_hal_open(devIndex, baudrate) == 0)			// serial open 
		return 0;

	CommStatus = COMM_RXSUCCESS;						// Comm status 
	BusUsing = 0;
	return 1;

}// Comm init

void __stdcall Comm::com_terminate() {
	dxl_hal_close();
}// Comm close

void __stdcall Comm::dxl_tx_packet() {
	
	unsigned char i;
	unsigned char TxNumByte, RealTxNumByte;
	unsigned char checksum = 0;

	if (BusUsing == 1)
		return;

	BusUsing = 1;

	if (InstructionPacket[LENGTH] > (MAXIMUM_TX + 2)){	// packet maximum exceed
		CommStatus = COMM_TXERROR;
		BusUsing = 0;
		return;
	}

	if (InstructionPacket[INSTRUCTION] != INST_PING
		&& InstructionPacket[INSTRUCTION] != INST_READ
		&& InstructionPacket[INSTRUCTION] != INST_WRITE
		&& InstructionPacket[INSTRUCTION] != INST_REG_WRITE
		&& InstructionPacket[INSTRUCTION] != INST_ACTION
		&& InstructionPacket[INSTRUCTION] != INST_RESET
		&& InstructionPacket[INSTRUCTION] != INST_SYNC_WRITE)
	{
		CommStatus = COMM_TXERROR;
		BusUsing = 0;
		return;
	}													// packet INST check

	InstructionPacket[0] = 0xff;						
	InstructionPacket[1] = 0xff;						
	InstructionPacket[2] = 0xff;						// start signal
	
	for (i = 0; i<(InstructionPacket[LENGTH]); i++)
		checksum += InstructionPacket[i + 5];			// ?

	InstructionPacket[InstructionPacket[LENGTH] + 5] = checksum;		// ?

	if (CommStatus == COMM_RXTIMEOUT || CommStatus == COMM_RXCORRUPT)
		dxl_hal_clear();								// Error 처리

	TxNumByte = InstructionPacket[LENGTH] + 4;
	RealTxNumByte = dxl_hal_tx((unsigned char*)InstructionPacket, TxNumByte);


	if (TxNumByte != RealTxNumByte){
		CommStatus = COMM_TXFAIL;
		BusUsing = 0;
		return;
	}

	if (InstructionPacket[INSTRUCTION] == INST_READ)
		dxl_hal_set_timeout(InstructionPacket[PARAMETER + 1] + 6);	
	else
		dxl_hal_set_timeout(6);										// ?

	CommStatus = COMM_TXSUCCESS;

}//comm tx packet

void __stdcall Comm::dxl_rx_packet()
{
	unsigned char i, j, nRead;
	unsigned char checksum = 0;

	if (BusUsing == 0)
		return;

	/*
	if (InstructionPacket[ID] == BROADCAST_ID)
	{
		CommStatus = COMM_RXSUCCESS;
		BusUsing = 0;
		return;
	}*/

	if (CommStatus == COMM_TXSUCCESS)
	{
		RxGetLength = 0;
		RxPacketLength = 5;
	}// tx 완료 시, 받을 준비

	nRead = dxl_hal_rx((unsigned char*)&StatusPacket[RxGetLength], RxPacketLength - RxGetLength);
	RxGetLength += nRead;

	if (RxGetLength < RxPacketLength)
	{
		if (dxl_hal_timeout() == 1)
		{
			if (RxGetLength == 0)
				CommStatus = COMM_RXTIMEOUT;
			else
				CommStatus = COMM_RXCORRUPT;
			BusUsing = 0;
			return;
		}
	}// time out 처리

	// Find packet header RxGetLength : entire data length
	for (i = 0; i<(RxGetLength - 1); i++)
	{
		if (StatusPacket[i] == 0xff && StatusPacket[i + 1] == 0xff && StatusPacket[i +2] == 0xff)
		{
			break;
		}
		else if (i == RxGetLength - 2 && StatusPacket[RxGetLength - 1] == 0xff)
		{
			break;
		}
	}

	if (i > 0)
	{
		for (j = 0; j<(RxGetLength - i); j++)
			StatusPacket[j] = StatusPacket[j + i];

		RxGetLength -= i;
	}

	if (RxGetLength < RxPacketLength)
	{
		CommStatus = COMM_RXWAITING;
		return;
	}

	// Check id pairing
	if (InstructionPacket[ID] != StatusPacket[ID])
	{
		CommStatus = COMM_RXCORRUPT;
		BusUsing = 0;
		return;
	}

	RxPacketLength = StatusPacket[LENGTH] + 5;
	if (RxGetLength < RxPacketLength)
	{
		nRead = dxl_hal_rx((unsigned char*)&StatusPacket[RxGetLength], RxPacketLength - RxGetLength);
		RxGetLength += nRead;
		if (RxGetLength < RxPacketLength)
		{
			CommStatus = COMM_RXWAITING;
			return;
		}
	}

	// Check checksum
	for (i = 0; i<(StatusPacket[LENGTH] + 1); i++)
		checksum += StatusPacket[i + 5];
	//checksum = ~checksum;

	if (StatusPacket[StatusPacket[LENGTH] + 3] != checksum)
	{
		CommStatus = COMM_RXCORRUPT;
		BusUsing = 0;
		return;
	}

	CommStatus = COMM_RXSUCCESS;
	BusUsing = 0;

}// rx packet process

void __stdcall Comm::dxl_txrx_packet()
{
	dxl_tx_packet();

	if (CommStatus != COMM_TXSUCCESS)
		return;

	do {
		dxl_rx_packet();
	} while (CommStatus == COMM_RXWAITING);
}

int __stdcall Comm::dxl_get_result(){
	return CommStatus;
}// packet status return

void __stdcall Comm::dxl_set_txpacket_id(int id)
{
	InstructionPacket[ID] = (unsigned char)id;
}// tx id set

void __stdcall Comm::dxl_set_txpacket_length(int length)
{
	InstructionPacket[LENGTH] = (unsigned char)length;
}// tx length set (LENGTH = 3)

void __stdcall Comm::dxl_set_txpacket_instruction(int instruction)
{
	InstructionPacket[INSTRUCTION] = (unsigned char)instruction;
}// tx instruction set (INS = 4)

void __stdcall Comm::dxl_set_txpacket_parameter(int index, int value)
{
	InstructionPacket[PARAMETER + index] = (unsigned char)value;
}// tx param set (PARAMETER = 5)

void __stdcall Comm::dxl_set_txpacket_minus(void)
{
	if(minus_check == 0)
		InstructionPacket[MINUS] = 0;
	else 
		InstructionPacket[MINUS] = 1;
}// tx Minus check set (MINUS = 8)




int __stdcall Comm::dxl_get_rxpacket_error(int errbit)
{
	if (StatusPacket[ERRBIT] & (unsigned char)errbit)
		return 1;

	return 0;
}// rx packet error check

int __stdcall Comm::dxl_get_rxpacket_length()
{
	return (int)StatusPacket[LENGTH];
}// rx get length

int __stdcall Comm::dxl_get_rxpacket_parameter(int index)
{
	return (int)StatusPacket[PARAMETER + index];
}// rx get param

int __stdcall Comm::dxl_makeword(int lowbyte, int highbyte)
{
	unsigned short word;

	word = highbyte;
	word = word << 8;
	word = word + lowbyte;
	return (int)word;
}

int __stdcall Comm::dxl_get_lowbyte(int word)
{
	unsigned short temp;

	temp = word & 0xff;
	return (int)temp;
}

int __stdcall Comm::dxl_get_highbyte(int word)
{
	unsigned short temp;

	temp = word & 0xff00;
	temp = temp >> 8;
	return (int)temp;
}

int __stdcall Comm::packet_make_3bytes(int byte_1, int byte_2 , int byte_3)
{
	int32_t temp;
	if(minus_check == 0)
		temp = (byte_3 << 16) + (byte_2 << 8) + byte_1;
	else {
		temp = -((byte_3 << 16) + (byte_2 << 8) + byte_1);
		minus_check = 0;
	}
		
	return (int)temp;
}

int __stdcall Comm::packet_get_3byte(int bit_32)
{
	uint32_t temp;
	if ((bit_32 & 0x80000000) == 0) {
		temp = bit_32 & 0xff0000;
		temp = temp >> 16;
	}
	else {
		bit_32 = ~(bit_32)+1;
		temp = bit_32 & 0xff0000;
		temp = temp >> 16;
	}

	return (int)temp;
}

int __stdcall Comm::packet_get_2byte(int bit_32)
{
	uint32_t temp;

	if ((bit_32 & 0x80000000) == 0) {
		temp = bit_32 & 0xff00;
		temp = temp >> 8;
	}
	else {
		bit_32 = ~(bit_32)+1;
		temp = bit_32 & 0xff00;
		temp = temp >> 8;
	}
	return (int)temp;
}

int __stdcall Comm::packet_get_1byte(int bit_32)
{
	uint32_t temp;

	if ((bit_32 & 0x80000000) == 0) {
		temp = bit_32 & 0xff;
	}
	else {
		minus_check = 1;
		bit_32 = ~(bit_32)+1;
		temp = bit_32 & 0xff;
	}
	return (int)temp;
}


void test(void) {
	printf("test!\n");
}