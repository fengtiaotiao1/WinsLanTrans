#pragma once

#include "winsock2.h"
#include <string>
#include <vector>

using namespace std;

typedef struct stSocketInfo {
	char *path;
	char *ip;
	SOCKET socket;
}SocketInfo, *PSocketInfo;
class Utils {
public: 
	static vector<string> split(const string &str, const string &delim);
	static string getLocalIp(SOCKET sockfd);
	static int calculateProcess(unsigned long processSize, unsigned long fileSize);
	static void reverseByte(string s, int n);
	

};