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
	if (!serial.OpenPort(portNum)) //��Ʈ�� �����ϰ� ���¿� �����Ͽ����� fail�� ��ȯ�Ѵ�.
		return RETURN_FAIL;

	serial.ConfigurePort(CBR_9600, 8, FALSE, NOPARITY, ONESTOPBIT); //��Ʈ �⺻���� �����Ѵ�.
	serial.SetCommunicationTimeouts(0, 0, 0, 0, 0); //Timeout�� ����

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
		// ���� ����Ʈ�� �� ���Ͽ� �� ����Ʈ�� �д� �Լ��� �ߺ� ȣ��
		serial.ReadByte(pPacket[i]); // ���� ���� �Լ� ȣ��
		dwBytesRead++;
	}

	return((int)dwBytesRead);  // ���⿡ ������ ����Ʈ �� ����
}

int CSerialComm::sendCommand(char pos) //�����͸� �����ϴ� �Լ�
{
	if (serial.WriteByte(pos)) //WriteByte()�� ���� ������ ���ۿ� �����ϸ� SUCCESS, �����ϸ� FAIL�� ��ȯ�Ѵ�.
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}

void CSerialComm::disconnect() //��Ʈ�� �� ���� ���� �ݴ� �Լ�
{
	serial.ClosePort();
}