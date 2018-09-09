#pragma once

#ifndef __TEST_HEADER
#define __TEST_HEADER

#endif

#include "OpenComm.h"  //serial port header

#define MAXIMUM_TX 150		// TX Packet 길이
#define MAXIMUM_RX 150		// RX Packet 길이

/*
#define ID					(2)
#define LENGTH				(3)
#define INSTRUCTION			(4)
#define ERRBIT				(4)
#define PARAMETER			(5)
#define DEFAULT_BAUDNUMBER	(1)
*/

#define ID					(3)
#define LENGTH				(4)
#define INSTRUCTION			(5)
#define ERRBIT				(5)
#define PARAMETER			(6)
#define MINUS				(8)
#define DEFAULT_BAUDNUMBER	(1)

#define INST_PING			(1)
#define INST_READ			(2)
#define INST_WRITE			(3)
#define INST_REG_WRITE		(4)
#define INST_ACTION			(5)
#define INST_RESET			(6)
#define INST_SYNC_WRITE		(131)


void test(void);	// test 용 printf 함수


typedef enum INST_{
	INST_READ_PULSE
	//INST_RESET
}INST;				// instructions

typedef enum CommResult_ {
	COMM_TXSUCCESS,
	COMM_RXSUCCESS,
	COMM_TXFAIL,
	COMM_RXFAIL,
	COMM_TXERROR,
	COMM_RXWAITING,
	COMM_RXTIMEOUT,
	COMM_RXCORRUPT
}CommResult;		// Communication Results


class Comm : public Cdxl {

public:
	Comm();

	///////////// device control methods ////////////////////////
	int __stdcall com_initialize(int devIndex, int baudnum);
	void __stdcall com_terminate();

	////////// packet communication methods ///////////////////////
	void __stdcall dxl_tx_packet();
	void __stdcall dxl_rx_packet();
	void __stdcall dxl_txrx_packet();
	int __stdcall dxl_get_result();

	///////////// set/get packet methods //////////////////////////
	void __stdcall dxl_set_txpacket_id(int id);
	void __stdcall dxl_set_txpacket_length(int length);
	void __stdcall dxl_set_txpacket_instruction(int instruction);
	void __stdcall dxl_set_txpacket_parameter(int index, int value);
	void __stdcall dxl_set_txpacket_minus(void);
	

	int __stdcall dxl_get_rxpacket_error(int errbit);
	int __stdcall dxl_get_rxpacket_parameter(int index);
	int __stdcall dxl_get_rxpacket_length();

	// utility for value
	int __stdcall dxl_makeword(int lowbyte, int highbyte);
	int __stdcall dxl_get_lowbyte(int word);
	int __stdcall dxl_get_highbyte(int word);

	int __stdcall packet_make_3bytes(int byte_1, int byte_2, int byte_3);
	int __stdcall packet_get_3byte(int bit_32);
	int __stdcall packet_get_2byte(int bit_32);
	int __stdcall packet_get_1byte(int bit_32);


	unsigned char InstructionPacket[MAXIMUM_TX+10];		// tx packet array
	unsigned char StatusPacket[MAXIMUM_RX + 10];		// rx packet array
	unsigned char RxPacketLength;						// rx packet length 
	unsigned char RxGetLength;							// rx length get
	int CommStatus;										// packet status 
	int BusUsing;										// bus status

	int minus_check;									// 3byte minus check
};
 