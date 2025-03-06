/*****************************************************************
* FileName:common_def.h
* Summary :
* Date	  :2024-4-30
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#define DB_THREAD_MAX				(8)			//db线程数
#define YEARSEC						(31536000)
#define MAX_SOCKET_NUM				(65536)
#define UID_LENGTH			 		(64)    // 帐号长度
#define NAME_LENGTH          		(32)    // 名字长度
// 管道标识符
enum enLockIdx
{
	IDX_PIPELOCK_C2S = 0,
	IDX_PIPELOCK_S2C = 1,
	IDX_PIPELOCK_A2C = 2,
	IDX_PIPELOCK_S2L = 3,
};

enum eQueueErrorCode
{
	QUEUE_OK = 0,     // param error
	QUEUE_PARAM_ERROR = -1,     // param error
	QUEUE_NO_SPACE = -2,        // message queue has no space
	QUEUE_NO_MESSAGE = -3,      // message queue has no message
	QUEUE_DATA_SEQUENCE_ERROR = -4,// message queue the message sequence error
};

enum eQueueModel
{
	ONE_READ_ONE_WRITE,   //一个进程读消息一个进程写消息
	ONE_READ_MUL_WRITE,   //一个进程读消息多个进程写消息
	MUL_READ_ONE_WRITE,   //多个进程读消息一个进程写消息
	MUL_READ_MUL_WRITE,   //多个进程读消息多个进程写消息
};

enum eShmModule
{
	SHM_INVALID = -1,
	SHM_INIT,     //第一次申请共享内存，初始化
	SHM_RESUME,   //共享内存已存在，恢复重新映射共享内存数据
};

#endif //__COMMON_DEF_H__
