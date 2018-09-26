#include "stdafx.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <iomanip>
#include <exception>
#include "md5.h"

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

string Utils::getFileMd5(const char * filePath)
{
	std::ifstream fin(filePath, std::ifstream::in | std::ifstream::binary);
	if (fin)
	{
		MD5_CTX context;
		MD5Init(&context);

		fin.seekg(0, fin.end);
		const auto fileLength = fin.tellg();
		fin.seekg(0, fin.beg);

		const int bufferLen = 8192;
		std::unique_ptr<unsigned char[]> buffer{ new unsigned char[bufferLen] {} };
		unsigned long long totalReadCount = 0;
		decltype(fin.gcount()) readCount = 0;
		// ��ȡ�ļ����ݣ�����MD5Update()����MD5ֵ
		while (fin.read(reinterpret_cast<char*>(buffer.get()), bufferLen))
		{
			readCount = fin.gcount();
			totalReadCount += readCount;
			MD5Update(&context, buffer.get(), static_cast<unsigned int>(readCount));
		}
		// �������һ�ζ���������
		readCount = fin.gcount();
		if (readCount > 0)
		{
			totalReadCount += readCount;
			MD5Update(&context, buffer.get(), static_cast<unsigned int>(readCount));
		}
		fin.close();

		// ���������Լ��
		if (totalReadCount != fileLength)
		{
			std::ostringstream oss;
			oss << "FATAL ERROR: read " << filePath << " failed!" << std::ends;
			throw std::runtime_error(oss.str());
		}

		unsigned char digest[16];
		MD5Final(digest, &context);

		// ��ȡMD5
		std::ostringstream oss;
		for (int i = 0; i < 16; ++i)
		{
			oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(digest[i]);
		}
		oss << std::ends;

		return std::move(oss.str());
	}
	else
	{
		std::ostringstream oss;
		oss << "FATAL ERROR: " << filePath << " can't be opened" << std::ends;
		throw std::runtime_error(oss.str());
	}
}
