#pragma once

#include <string>
#include "winsock2.h"

using namespace std;

class TcpClient {
public:
    static void initClientSocket(const char* address, const char* path);
	static DWORD WINAPI recvData(LPVOID socket);
    static void sendFileProcess(int state, int process, const char* filename);
};

