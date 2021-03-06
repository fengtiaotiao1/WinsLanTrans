// MyDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "udp.h"
#include "tcpclient.h"
#include "tcpserver.h"
#include "windll.h"
#include "const.h"

extern "C" {
#include "sqlitehelp.h"
#include "groupdb.h"
}

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif 

using namespace std;

UdpCallback g_UdpNotify = NULL;
FileCallback g_FileNotify = NULL;

DLLEXPORT void initSocket()
{
	UDP::initUdp();
	TcpServer::initServerSocket();

}

DLLEXPORT void sendUdpData(int cmd, const char* destAddr, const char* msg)
{
	UDP::sendUdpData(cmd, destAddr, msg);
}

DLLEXPORT void onlineNotify()
{
	UDP::sendUdpData(CMD_TYPE_ONLINE, BROADCAST_ADDR, NULL_MSG);
}

DLLEXPORT void offlineNotify()
{
	UDP::sendUdpData(CMD_TYPE_OFFLINE, BROADCAST_ADDR, NULL_MSG);
}

DLLEXPORT void sendFileReq(const char *destAddr, const char *msg)
{
	UDP::sendUdpData(CMD_TYPE_SEND_FILE, destAddr, msg);
}

DLLEXPORT void sendFileRecvResp(const char *destAddr, const char *msg)
{
	UDP::sendUdpData(CMD_TYPE_RECV_FILE, destAddr, msg);
}

DLLEXPORT void sendFileRejectResp(const char *destAddr, const char *msg)
{
	UDP::sendUdpData(CMD_TYPE_REJECT_FILE, destAddr, msg);
}

DLLEXPORT void release()
{
	UDP::release();
	TcpServer::release();
}

DLLEXPORT void startReceive(const char *path)
{
	TcpServer::startReceive(path);
}

DLLEXPORT void sendFile(const char *address, const char *path)
{
	TcpClient::initClientSocket(address, path);
}

DLLEXPORT void setUdpNotify(void *callback) {
	g_UdpNotify = (UdpCallback)callback;
}

DLLEXPORT void setFileNotify(void *callback) {
	g_FileNotify = (FileCallback)callback;
}

DLLEXPORT int initDb(const char * dbPath)
{
	return openLanTransDb(dbPath);
}

DLLEXPORT int addGroupMember(int groupId, const char *memberName, const char *memberIp)
{
	PGroupInfo groupInfo = (PGroupInfo)malloc(sizeof(GroupInfo));
	groupInfo->group_id = groupId;
	strcpy(groupInfo->member_name, memberName);
	strcpy(groupInfo->member_addr, memberIp);
	int ret = addGroupMemberData(groupInfo);
	free(groupInfo);
	return ret;
}

DLLEXPORT int deleteGroupMember(int groupId, const char *memberName)
{
	PGroupInfo groupInfo = (PGroupInfo)malloc(sizeof(GroupInfo));
	groupInfo->group_id = groupId;
	strcpy(groupInfo->member_name, memberName);
	int ret = deleteGroupMemberData(groupInfo);
	free(groupInfo);
	return ret;
}

DLLEXPORT int deleteGroupMemberByName(const char *memberName)
{
	return deleteMemberDataByName(memberName);
}

DLLEXPORT int deleteGroupById(int groupId)
{
	return deleteGroupDataById(groupId);
}

DLLEXPORT int deleteGroups()
{
	return deleteAllGroupData();
}

DLLEXPORT int updateMemberIpByName(const char *memberName, const char *newIp)
{
	return updateMemberDataByName(memberName, newIp);
}

DLLEXPORT int queryAllGroup()
{
	return 0;
}

DLLEXPORT int queryGroupMemberById(int groupId)
{
	return 0;
}

void notify(int cmd, const char *srcAddr, const char *msg) {
	g_UdpNotify(cmd, srcAddr, msg);
}

void fileTransCallback(int state, int type, int process, const char *filename) {
	g_FileNotify(state, type, process, filename);
}


