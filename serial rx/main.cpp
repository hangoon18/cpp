#include <iostream>
#include "Serialcomm.h"


using namespace std;

int main() {

	
	CSerialComm serialComm; //SerialComm ��ü ����
	CSerialPort serial;
	
	BYTE* ab = new BYTE[5];
	unsigned char ca[4];

	if (!serialComm.connect("COM1")) //COMx���� ��Ʈ�� �����Ѵ�. ������ ��� -1�� ��ȯ�Ѵ�.
	{
		cout << "connect faliled" << endl;
		return -1;
	}
	else
		cout << "connect successed" << endl;

	while (1) { //���¿� ������ ��� ReadByte()�� ���� ��������� �����͸� �����Ѵ�. ���ſ� ���� �� ��� failed �޽����� ����Ѵ�.
		
		serialComm.RxPacket(ca,10);
		//ca[2] = ';
		for (int i = 0; i < 10; i++)
			printf("%d ", (int)ca[i]);
		printf("\n");
	
	}

	serialComm.disconnect(); //�۾��� ������ ��Ʈ�� �ݴ´�
	return 0;
}