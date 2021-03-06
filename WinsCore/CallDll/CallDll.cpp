// CallDll.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "windll.h"
#include <iostream>
#include <string> 
#include "const.h"

using namespace std;

void udpNotify(int cmd, const char* srcAddr, const char* msg) {
	printf("udpNotify: cmd:%d, srcAddr:%s, msg:%s\n", cmd, srcAddr, msg);
	if (cmd == CMD_TYPE_RECV_FILE){
		sendFile(srcAddr, "F:/file/windows7.iso");
	}
}

void fileNotify(int state, int type, int process, const char* filename) {
	printf("fileNotify: state:%d, type:%d, process:%d, filename:%s\n", state, type, process, filename);
}

int main()
{
    std::cout << "Hello World!\n"; 

	setUdpNotify(udpNotify);
	setFileNotify(fileNotify);

	initSocket();
	initDb("Group.db");

	printf("输入4-发送接收文件响应\n");
	printf("输入5-发送文件请求\n");
	printf("输入6-发送文件\n");

	int cmd;
	string ip;
	//cin >> cmd >> ip;

	while(cin >> cmd >> ip){
		if (cmd == 4) {
			sendUdpData(4, ip.c_str(), " ");
			startReceive("F:/file");
		}
		else if (cmd == 5) { 
			sendUdpData(6, ip.c_str(), " ");
		}
		else if (cmd == 6) {
			sendFile(ip.c_str(), "F:/file/windows7.iso");
		}
		
	}

	/*int value = 0;
	char temp = 0;
	while ((temp = getchar()) < '9') {
		value = temp - '0';
		if (value == 4)
		{
			sendUdpData(4, "192.168.150.128", " ");
			startReceive("F:/file");
		}
		else if (value == 5)
		{
			sendUdpData(6, "192.168.150.128", " ");																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																							 
		}
		else if (value == 6)
		{
			sendFile("192.168.150.128", "F:/windows7.iso");
		}
		else if (value == 1) {
			addGroupMember(1, "yuanjf", "192.168.150.121");
			addGroupMember(1, "wang", "192.168.150.111");
			addGroupMember(1, "liahha", "192.168.150.101"); 
			addGroupMember(2, "liahha", "192.168.150.101");
			addGroupMember(2, "zhangsan", "192.168.140.101");
			addGroupMember(3, "zhangsan", "192.168.140.101");
			addGroupMember(3, "wugen", "192.168.130.101");
			addGroupMember(3, "liahha", "192.168.150.101");
		}
		else if (value == 2) {
			deleteGroupMember(1, "yuanjf");
		}
		else if (value == 3) {
			updateMemberIpByName("zhangsan", "199.168.130.101");
		}
		else if (value == 0) {
			deleteGroupById(2);
		}
		else if (value == 7) {
			deleteGroupMemberByName("liahha");
		}
		else if (value == 8) {
			deleteGroups();
		}
	}*/
	
	system("pause");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
