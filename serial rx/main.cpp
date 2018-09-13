#include <iostream>
#include "Serialcomm.h"


using namespace std;

int main() {

	
	CSerialComm serialComm; //SerialComm 객체 생성
	CSerialPort serial;
	
	BYTE* ab = new BYTE[5];
	unsigned char ca[4];

	if (!serialComm.connect("COM1")) //COMx번의 포트를 오픈한다. 실패할 경우 -1을 반환한다.
	{
		cout << "connect faliled" << endl;
		return -1;
	}
	else
		cout << "connect successed" << endl;

	while (1) { //오픈에 성공한 경우 ReadByte()를 통해 계속적으로 데이터를 수신한다. 수신에 실패 할 경우 failed 메시지를 출력한다.
		
		serialComm.RxPacket(ca,10);
		//ca[2] = ';
		for (int i = 0; i < 10; i++)
			printf("%d ", (int)ca[i]);
		printf("\n");
	
	}

	serialComm.disconnect(); //작업이 끝나면 포트를 닫는다
	return 0;
}