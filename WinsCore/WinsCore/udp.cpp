#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include "udp.h"
#include "udpmsg.h"
#include "const.h"
#include "utils.h"
#include "stdafx.h"
#include "winsock2.h"
#include "windll.h"
#pragma comment(lib, "ws2_32.lib")

SOCKET sockfd;
string localIp;
bool isRecv = true;

void UDP::initUdp() {
	WSADATA WSAData;
	//初始化/
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		printf("winsock init failed\n");
		return;
	}
    //创建socket/
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {
		printf("create socket error.\n");
        return;
    }
    //允许发送广播数据/
    const int opt = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *) &opt, sizeof(opt));
    if (ret < 0) {
		printf("set socket option error...\n");
        ::closesocket(sockfd);
        return;
    }
    //启动udp接收线程/
	HANDLE udpThread = CreateThread(NULL, 0, recvData, (LPVOID)sockfd, 0, 0);
	if (udpThread != NULL) {
		CloseHandle(udpThread);
	}
  
    //获取本机IP/
    localIp = Utils::getLocalIp(sockfd);
    sendUdpData(CMD_TYPE_ONLINE, BROADCAST_ADDR, NULL_MSG);
}

void UDP::sendUdpData(int cmd, const char* destAddr, const char* msg) {
    UdpMsg udpMsg;
    udpMsg.setSrcAddr(localIp);
    udpMsg.setDestAddr(destAddr);
    udpMsg.setPort(PORT);
    udpMsg.setCmd(cmd);
    udpMsg.setMsg(msg);

	sockaddr_in addrto;
	memset(&addrto, 0, sizeof(addrto));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = inet_addr(udpMsg.getDestAddr().c_str());
	addrto.sin_port = htons(udpMsg.getPort());
	char sendMsg[1024];
	strcpy(sendMsg, udpMsg.toString().c_str());
	printf("send msg is: %s\n", sendMsg);
	int ret = sendto(sockfd, sendMsg, (int)strlen(sendMsg), 0, (sockaddr *)&addrto, sizeof(addrto));
	if (ret < 0) {
		printf("send data error....\n");
	}
}

DWORD __stdcall UDP::recvData(LPVOID socket) {
	SOCKET sockfd = (SOCKET)socket;
	struct sockaddr_in addrto;
	memset(&addrto, 0, sizeof(addrto));
	addrto.sin_family = AF_INET;
	addrto.sin_port = htons(PORT);
	addrto.sin_addr.s_addr = 0x00000000;

	if (bind(sockfd, (struct sockaddr *) &(addrto), sizeof(struct sockaddr_in)) == -1) {
		printf("udp bind error...\n");
		return 0;
	}
	int len = sizeof(sockaddr_in);
	char readMsg[1024] = { 0 };

	while (isRecv) {
		memset(readMsg, 0, sizeof(readMsg));
		int ret = recvfrom(sockfd, readMsg, sizeof(readMsg), 0, (struct sockaddr *) &addrto, &len);
		string fromAddr = inet_ntoa(addrto.sin_addr);
		printf("receive from %s, msg is: %s\n", fromAddr.c_str(), readMsg);
		if (ret <= 0) {
			printf("read data error....\n");
		}
		else {
			if (strcmp(fromAddr.c_str(), localIp.c_str()) == 0) {
				continue;
			}
			vector<string> array = Utils::split(readMsg, ":");
			int cmd = atoi(array[array.size() - 2].c_str());
			printf("udp received cmd is: %d\n", cmd);
			string msg = array[array.size() - 1];
			if (cmd == CMD_TYPE_ONLINE) {
				sendUdpData(CMD_TYPE_ANSWER, inet_ntoa(addrto.sin_addr), (char *)NULL_MSG);
			}
			//发送消息通知/
			sendMsgNotify(cmd, fromAddr.c_str(), msg.c_str());
		}
		Sleep(1);
	}
	return 0;
}

void UDP::sendMsgNotify(int cmd, const char* srcAddr, const char* msg) {
    notify(cmd, srcAddr, msg);
}

void UDP::release() {
    if (sockfd >= 0) {
        closesocket(sockfd);
    }
	WSACleanup();
}
