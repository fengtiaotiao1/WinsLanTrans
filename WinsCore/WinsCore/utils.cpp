#include "stdafx.h"
#include "utils.h"

vector<string> Utils::split(const string &str, const string &delim) {
	vector<string> res;
	if ("" == str) return res;
	//�Ƚ�Ҫ�и���ַ�����string����ת��Ϊchar*����
	char *strs = new char[str.length() + 1];
	strcpy(strs, str.c_str());

	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //�ָ�õ����ַ���ת��Ϊstring����
		res.push_back(s); //����������
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

// �Զ��巴ת�ֽ�
void Utils::reverseByte(string s, int n) {
	for (int i = 0, j = n - 1; i < j; i++, j--) {
		char c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}