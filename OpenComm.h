#pragma once

#ifndef OPEN_COMM
#define OPEN_COMM

#endif

#include <windows.h>
class Cdxl {

public:
	Cdxl();

	int dxl_hal_open(int devIndex, float baudrate);
	void dxl_hal_close();
	void dxl_hal_clear();
	int dxl_hal_tx(unsigned char *pPacket, int numPacket);
	int dxl_hal_rx(unsigned char *pPacket, int numPacket);
	void dxl_hal_set_timeout(int NumRcvByte);
	int dxl_hal_timeout();

	
	HANDLE ghSerial_Handle;
	float gfByteTransTime;
	float gfRcvWaitTime;
	LARGE_INTEGER gStartTime;
};
