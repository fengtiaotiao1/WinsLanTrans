#include "stdafx.h"
#include "tcpclient.h"
#include "utils.h"
#include "const.h"
#include <sstream>
#include "windll.h"
#pragma comment(lib, "ws2_32.lib")

static int sendProcess = -1;

void TcpClient::initClientSocket(const char* address, const char* path)
{
	WSADATA WSAData;
	//初始化/
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		printf("winsock init failed\n");
		return;
	}
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket < 0) {
		printf("client socket create error!\n");
	}

	const int timeout = 10000;
	int ret = setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

	//    int flags = fcntl(clientSocket, F_GETFL, 0);
	//    fcntl(clientSocket, F_SETFL, flags & ~O_NONBLOCK);    //设置成阻塞模式；

		//启动接收线程:接收来自服务端的消息
	PSocketInfo socketInfo = (PSocketInfo)malloc(sizeof(SocketInfo));
	socketInfo->path = (char *)malloc(strlen(path) + 1);
	strcpy(socketInfo->path, path);
	socketInfo->ip = (char *)malloc(strlen(address) + 1);
	strcpy(socketInfo->ip, address);
	socketInfo->socket = clientSocket;
	//启动线程
	HANDLE tcpThread = CreateThread(NULL, 0, recvData, (LPVOID)socketInfo, 0, 0);
	if (tcpThread != NULL) {
		CloseHandle(tcpThread);
	}
}

DWORD __stdcall TcpClient::recvData(LPVOID socket)
{
	PSocketInfo socketInfo = (PSocketInfo)socket;
	SOCKET clientSocket = socketInfo->socket;
	string path = socketInfo->path;
	string address = socketInfo->ip;

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(FILE_TRANS_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(address.c_str());

	printf("start connect server, address is : %s\n", address.c_str());
	int ret = connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	if (ret == -1) {
		printf("connect client is : %d\n", ret);
		closesocket(clientSocket);
		return 0;
	}
	printf("client connect success\n");

	//获取文件名
	vector<string> array = Utils::split(path, "/\\");
	printf("yuanjf== array: %d\n", array.size());
	if (array.size() == 0) {
		closesocket(clientSocket);
		return 0;
	}
	string fileName = array[array.size() - 1];
	//获取文件大小
	struct _stati64 st;
	if (_stati64(path.c_str(), &st) < 0) {
		closesocket(clientSocket);
		return 0;
	}
	unsigned long fileSize = st.st_size;
	stringstream fileInfo;
	fileInfo << SEND_FILEINFO
		<< ":"
		<< fileName
		<< ":"
		<< fileSize;

	printf("send fileInfo: %s\n", fileInfo.str().c_str());
	send(clientSocket, fileInfo.str().c_str(), static_cast<int>(fileInfo.str().size()), 0);
	//向服务端发送文件名和文件大小
	char buffer[1024];
	memset(&buffer, 0, sizeof(buffer));
	unsigned long fileOffset = 0;
	while (recv(clientSocket, buffer, sizeof(buffer), 0) > 0) {
		printf("from server msg: %s\n", buffer);
		vector<string> array = Utils::split(buffer, ":");
		if (!strcmp(array[0].c_str(), READY_TO_RECEIVE)) {
			fileOffset = atoi(array[1].c_str());
			break;
		}
	}
	//向服务端发送文件
	FILE *fp = fopen(path.c_str(), "rb");
	if (fp == NULL) {
		printf("file open failed\n");
		closesocket(clientSocket);
		return 0;
	}
	fseek(fp, fileOffset, SEEK_SET);
	memset(&buffer, 0, sizeof(buffer));
	sendFileProcess(TRANS_START, 0, fileName.c_str());
	unsigned long sendSize = 0;
	while ((ret = (int)fread(buffer, sizeof(char), sizeof(buffer), fp)) > 0) {
		if ((send(clientSocket, buffer, ret, 0)) < 0) {
			break;
		}
		sendSize += ret;
		sendFileProcess(TRANS_UPLOAD, Utils::calculateProcess(sendSize, fileSize), fileName.c_str());
		memset(&buffer, 0, sizeof(buffer));
	}
	fclose(fp);
	free(socketInfo->path);
	free(socketInfo->ip);
	free(socketInfo);
	closesocket(clientSocket);
	sendFileProcess(TRANS_SUCCESS, -1, fileName.c_str());
	printf("client send success\n");

	return 0;
}

void TcpClient::sendFileProcess(int state, int process, const char* filename)
{
	if (process != sendProcess) {
		sendProcess = process;
		fileTransCallback(state, TYPE_SEND, process, filename);
	}
}
