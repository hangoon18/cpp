#include "serialcomm.h"
#include <iostream>
using namespace std;

CSerialComm::CSerialComm() {
	memset(InstructionPacket, 0, MAXNUM_TXPARAM + 10);
	memset(StatusPacket, 0, MAXNUM_RXPARAM + 10);
	RxPacketLength = 0;
	RxGetLength = 0;
	//CommStatus = COMM_RXSUCCESS;
	BusUsing = 0;
}
CSerialComm::~CSerialComm() {}


int CSerialComm::connect(char* portNum)
{
	if (!serial.OpenPort(portNum)) //포트를 오픈하고 오픈에 실패하였으면 fail을 반환한다.
		return RETURN_FAIL;

	serial.ConfigurePort(CBR_9600, 8, FALSE, NOPARITY, ONESTOPBIT); //포트 기본값을 설정한다.
	serial.SetCommunicationTimeouts(0, 0, 0, 0, 0); //Timeout값 설정

	return RETURN_SUCCESS;
}



int CSerialComm::RxPacket(unsigned char *pPacket, int numPacket)
{
	// Transmiting date
	// *pPacket: data array pointer
	// numPacket: number of data array
	// Return: number of data transmitted. -1 is error.

	//if (!m_bPortOnOff || ghSerial_Handle == NULL) return(0);
	//if (m_hComm == NULL)
	//	return 0;

	DWORD dwBytesRead = 0;

	for (int i = 0; i<numPacket; i++) {
		// 여러 바이트를 읽 위하여 한 바이트씩 읽는 함수를 중복 호출
		serial.ReadByte(pPacket[i]); // 쓰기 위한 함수 호출
		dwBytesRead++;
	}

	return((int)dwBytesRead);  // 쓰기에 성공한 바이트 수 리턴
}

int CSerialComm::sendCommand(char pos) //데이터를 전송하는 함수
{
	if (serial.WriteByte(pos)) //WriteByte()를 통해 데이터 전송에 성공하면 SUCCESS, 실패하면 FAIL을 반환한다.
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}

void CSerialComm::disconnect() //포트를 다 쓰고 난뒤 닫는 함수
{
	serial.ClosePort();
}