#pragma once

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif 

typedef void( *UdpCallback)(int cmd, const char* srcAddr, const char* msg);
typedef void( *FileCallback)(int state, int type, int process, const char* filename);

extern "C" DLLEXPORT void initSocket();
extern "C" DLLEXPORT void sendUdpData(int cmd, const char* destAddr, const char* msg);
extern "C" DLLEXPORT void sendFile(const char* address, const char* path);
extern "C" DLLEXPORT void startReceive(const char* path);
extern "C" DLLEXPORT void release();
extern "C" DLLEXPORT void setUdpNotify(void *callback);
extern "C" DLLEXPORT void setFileNotify(void *callback);

extern "C" DLLEXPORT int initDb(const char *dbPath);
extern "C" DLLEXPORT int addGroupMember(int groupId, const char *memberName, const char *memberIp);
extern "C" DLLEXPORT int deleteGroupMember(int groupId, const char *memberName);
extern "C" DLLEXPORT int deleteGroupMemberByName(const char *memberName);
extern "C" DLLEXPORT int deleteGroupById(int groupId);
extern "C" DLLEXPORT int deleteGroups();
extern "C" DLLEXPORT int updateMemberIpByName(const char *memberName, const char *newIp);
extern "C" DLLEXPORT int queryAllGroup();
extern "C" DLLEXPORT int queryGroupMemberById(int groupId);

void notify(int cmd, const char* srcAddr, const char* msg);
void fileTransCallback(int state, int type, int process, const char* filename);
