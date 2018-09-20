#include "stdafx.h"
#include "utils.h"

vector<string> Utils::split(const string &str, const string &delim) {
	vector<string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型
	char *strs = new char[str.length() + 1];
	strcpy(strs, str.c_str());

	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //分割得到的字符串转换为string类型
		res.push_back(s); //存入结果数组
		p = strtok(NULL, d);
	}
	return res;
}

string Utils::getLocalIp(SOCKET sockfd) {
	string ip;
	char cHost[256];
	gethostname(cHost, sizeof(cHost));
	hostent *pHost = gethostbyname(cHost);
	in_addr addr;
	for (int i = 0;;i++) {
		printf("host name: %s\n", pHost->h_name);
		char *p = pHost->h_addr_list[i];
		if (p == NULL) {
			break;
		}
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);
		ip = inet_ntoa(addr);
	}
	return ip;
}

int Utils::calculateProcess(unsigned long processSize, unsigned long fileSize) {
	int ret = (int)((processSize * 1.0 / fileSize) * 100);
	if (ret >= 100) {
		return 100;
	}
	return ret;
}

// 自定义反转字节
void Utils::reverseByte(string s, int n) {
	for (int i = 0, j = n - 1; i < j; i++, j--) {
		char c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}