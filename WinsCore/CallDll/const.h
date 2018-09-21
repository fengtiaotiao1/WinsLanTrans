#pragma once

#define PORT                     6000
#define CMD_TYPE_ONLINE          1
#define CMD_TYPE_OFFLINE         2
#define CMD_TYPE_ANSWER          3
#define CMD_TYPE_RECV_FILE       4
#define CMD_TYPE_REJECT_FILE     5
#define CMD_TYPE_SEND_FILE       6
#define CMD_TYPE_SEND_CHAT_MSG   7

#define BROADCAST_ADDR "255.255.255.255"
#define NULL_MSG " "
#define DEFAULT_SAVE_PATH "/mnt/internal_sd/download/files/"
#define READY_TO_RECEIVE "START_SEND"
#define SEND_FILEINFO "FILEINFO"

#define USHORT u_int16_t
#define UINT u_int32_t
#define ULONG u_int64_t
#define UCHAR unsigned char
#define CHAR char
#define TRUE 1
#define FALSE 0

// Tcp
#define FILE_TRANS_PORT         3444

#define TYPE_SEND               7
#define TYPE_RECEIVE            8

#define TRANS_START             9
#define TRANS_FAILED            10
#define TRANS_SUCCESS           11
#define TRANS_UPLOAD            12

#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

typedef struct stGroupInfo {
	char member_name[128];
	char member_addr[128];
	int group_id;
} GroupInfo, *PGroupInfo;