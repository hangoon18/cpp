#pragma once
#include "serialport.h"


#define MAXNUM_TXPARAM		(150)
#define MAXNUM_RXPARAM		(60)

#define RETURN_SUCCESS 1
#define RETURN_FAIL 0


class CSerialComm
{
public:
	CSerialComm();
	~CSerialComm();

	CSerialPort	serial;
	int		connect(char* _portNum);
	int		RxPacket(unsigned char *pPacket, int numPacket);
	int		sendCommand(char pos);
	void	disconnect();

	unsigned char InstructionPacket[MAXNUM_TXPARAM + 10];
	unsigned char StatusPacket[MAXNUM_RXPARAM + 10];
	unsigned char RxPacketLength;
	unsigned char RxGetLength;
	int CommStatus;
	int BusUsing;

};