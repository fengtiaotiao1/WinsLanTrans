#pragma once

#include <string>
#include "winsock2.h"

using namespace std;

class TcpServer {
public:
    static void initServerSocket();
    static void startReceive(const char* path);
	static DWORD WINAPI recvData(LPVOID socket);
    static void sendFileProcess(int state, int process, const char* filename);
    static void release();
};

