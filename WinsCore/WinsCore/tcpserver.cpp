#include "stdafx.h"
#include "tcpserver.h"
#include "winsock2.h"
#include "const.h"
#include "utils.h"
#include "io.h"
#include "direct.h"
#include "windll.h"
#include <sstream>
#pragma comment(lib, "ws2_32.lib")

static SOCKET serverSocket;
static int recvProcess = -1;

void TcpServer::initServerSocket()
{
	WSADATA WSAData;
	//初始化/
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		printf("winsock init failed\n");
		return;
	}
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket < 0) {
		printf("socket create error\n");
	}
	const int opt = 1;
	const int timeout = 10000;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
	setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = 0x00000000;
	serverAddr.sin_port = htons(FILE_TRANS_PORT);

	if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(sockaddr_in))) {
		printf("tcp bind error\n");
		closesocket(serverSocket);
		return;
	}
	if (listen(serverSocket, 10)) {
		printf("server listen error\n");
		closesocket(serverSocket);
		return;
	}
}

void TcpServer::startReceive(const char* path)
{
	PSocketInfo socketInfo = (PSocketInfo)malloc(sizeof(SocketInfo));
	socketInfo->path = (char *)malloc(strlen(path) + 1);
	strcpy(socketInfo->path, path);
	socketInfo->socket = serverSocket;
	//启动接收线程
	HANDLE tcpThread = CreateThread(NULL, 0, recvData, (LPVOID)socketInfo, 0, 0);
	if (tcpThread != NULL) {
		CloseHandle(tcpThread);
	}
}

DWORD __stdcall TcpServer::recvData(LPVOID socket)
{
	PSocketInfo socketInfo = (PSocketInfo)socket;
	SOCKET serverSocket = socketInfo->socket;
	string path = socketInfo->path;

	struct sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	int len = sizeof(clientAddr);
	printf("waiting to receive connection\n");
	SOCKET clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &len);
	if (clientSocket == -1) {
		printf("accept error\n");
		return 0;
	}
	printf("client connect success\n");

	char buffer[1024];
	memset(&buffer, 0, sizeof(buffer));
	while (recv(clientSocket, buffer, sizeof(buffer), 0) > 0) {
		vector<string> array = Utils::split(buffer, ":");
		if (array.size() > 0 && !strcmp(array[0].c_str(), SEND_FILEINFO)) {
			printf("from client msg: %s\n", buffer);
			//向客户端发送准备好接收的消息
			//string msg = READY_TO_RECEIVE;
			//send(clientSocket, msg.c_str(), static_cast<int>(msg.size()), 0); 
			break;
		}
	}
	vector<string> array = Utils::split(buffer, ":");
	string fileName = array[1];
	unsigned long fileSize = atoi(array[2].c_str());
	string fileMd5 = array[3];
	memset(&buffer, 0, sizeof(buffer));

	if (_access(path.c_str(), 6) < 0) {
		_mkdir(path.c_str());
	}
	char *tempPath = (char *)malloc(path.size() + fileName.size() + fileMd5.size() + 7);
	strcpy(tempPath, path.c_str());
	strcat(tempPath, "/");
	strcat(tempPath, fileName.c_str());
	strcat(tempPath, "-");
	strcat(tempPath, fileMd5.c_str());
	strcat(tempPath, ".tmp");

	FILE *fp = fopen(tempPath, "ab+");
	if (fp == NULL) {
		printf("open file failed\n");
		//        close(serverSocket);
		return 0;
	}
	//判断当前文件的大小
	unsigned long fileOffsetSize;
	struct _stati64 st;
	if (_stati64(tempPath, &st) < 0) {
		fileOffsetSize = 0;
	}
	else {
		fileOffsetSize = st.st_size;
	}
	printf("current file offset is :%ld\n", fileOffsetSize);
	//当前文件移动到指定位置
	fseek(fp, fileOffsetSize, SEEK_SET);

	//向客户端发送准备好接收的消息（包含已接收的偏移量）
	stringstream readyRecvInfo;
	readyRecvInfo << READY_TO_RECEIVE
		<< ":"
		<< fileOffsetSize;
	send(clientSocket, readyRecvInfo.str().c_str(), static_cast<int>(readyRecvInfo.str().size()), 0);

	//接收到的消息存入文件
	sendFileProcess(TRANS_START, 0, fileName.c_str());
	int ret;
	unsigned long recvSize = fileOffsetSize;
	while ((ret = (int)recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
		if (fwrite(buffer, sizeof(char), ret, fp) <= 0) {
			break;
		}
		memset(&buffer, 0, sizeof(buffer));
		recvSize += ret;
		sendFileProcess(TRANS_UPLOAD, Utils::calculateProcess(recvSize, fileSize), fileName.c_str());
	}
	fclose(fp);
	closesocket(clientSocket);
	//判断文件是否完整接收，若接收完整则重命名
	if (recvSize == fileSize) {
		printf("file received complete!\n");
		if (checkFile(tempPath, fileName, fileMd5))
		{
			sendFileProcess(TRANS_SUCCESS, 101, fileName.c_str());
		}
		else {
			sendFileProcess(TRANS_FAILED, -1, fileName.c_str());
		}
	}
	free(tempPath);
	free(socketInfo->path);
	free(socketInfo);
	printf("server receive success\n");
	return 0;
}

bool TcpServer::checkFile(string tempPath, string fileName, string fileMd5)
{
	//判断接收的文件是否正确
	string recvMd5 = Utils::getFileMd5(tempPath.c_str());
	if (strcmp(recvMd5.c_str(), fileMd5.c_str()) != 0)
	{
		printf("received file failed!\n");
		remove(tempPath.c_str());
		return false;
	}
	string suffixStr = fileName.substr(fileName.find_last_of('.'));
	string renamePath = tempPath.substr(0, tempPath.find_last_of("-"));
	int i = 1;
	while (_access(renamePath.c_str(), 0) != -1) {
		//文件已存在
		renamePath = tempPath.substr(0, tempPath.find_last_of("-") - suffixStr.size());
		char index[5];
		renamePath.append("(")
			.append(_itoa(i, index, 10))
			.append(")")
			.append(suffixStr);
		i++;
	}
	if (rename(tempPath.c_str(), renamePath.c_str()) < 0) {
		printf("file rename failed!\n");
		perror("rename");
		return false;
	}
	return true;
}

void TcpServer::sendFileProcess(int state, int process, const char* filename)
{
	if (process != recvProcess) {
		recvProcess = process;
		fileTransCallback(state, TYPE_RECEIVE, process, filename);
	}
}

void TcpServer::release()
{
	if (serverSocket >= 0) {
		closesocket(serverSocket);
	}
	WSACleanup();
}
