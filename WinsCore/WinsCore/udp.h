#pragma once

#include <string>
#include "winsock2.h"
#include <windows.h>

using namespace std;

class UDP {
public:
	static void initUdp();
	static void sendUdpData(int cmd, const char* destAddr, const char* msg);
	static DWORD WINAPI recvData(LPVOID socket);
	static void sendMsgNotify(int cmd, const char* srcAddr, const char* msg);
	static void release();

};
