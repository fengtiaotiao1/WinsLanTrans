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
		// 读取文件内容，调用MD5Update()更新MD5值
		while (fin.read(reinterpret_cast<char*>(buffer.get()), bufferLen))
		{
			readCount = fin.gcount();
			totalReadCount += readCount;
			MD5Update(&context, buffer.get(), static_cast<unsigned int>(readCount));
		}
		// 处理最后一次读到的数据
		readCount = fin.gcount();
		if (readCount > 0)
		{
			totalReadCount += readCount;
			MD5Update(&context, buffer.get(), static_cast<unsigned int>(readCount));
		}
		fin.close();

		// 数据完整性检查
		if (totalReadCount != fileLength)
		{
			std::ostringstream oss;
			oss << "FATAL ERROR: read " << filePath << " failed!" << std::ends;
			throw std::runtime_error(oss.str());
		}

		unsigned char digest[16];
		MD5Final(digest, &context);

		// 获取MD5
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
