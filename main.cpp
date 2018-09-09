#include <iostream>
using namespace std;
#include "packet.h"
#include "OpenComm.h"	

int main(void) {

	printf("Hello World!\n");
	test();
	
	Comm comm;
	Cdxl cdxl;

	//comm.com_initialize(1, 115200);
	

	comm.dxl_set_txpacket_id(20);	
	comm.dxl_set_txpacket_length(4);
	comm.dxl_set_txpacket_instruction(INST_WRITE);

	comm.dxl_set_txpacket_parameter(0, comm.packet_get_1byte(100000));
	comm.dxl_set_txpacket_parameter(1, comm.packet_get_2byte(100000));
	comm.dxl_set_txpacket_parameter(2, comm.packet_get_3byte(100000));
	comm.dxl_set_txpacket_minus();
	
	comm.dxl_tx_packet();

	for(int i = 0; i<10;i++)
		cout << "Num [" <<i<<"] : "<<(int)comm.InstructionPacket[i] << endl;

	return 0;
}

