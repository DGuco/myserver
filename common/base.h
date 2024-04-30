//
//  base.h
//  游戏工具
//  Created by DGuco on 16/12/19.
//  Copyright ? 2016年 DGuco. All rights reserved.
//

#ifndef __BASE_H__
#define __BASE_H__

#include <vector>
#include <string>
#include <csignal>
#include <stdio.h>
#include "platform_def.h"

#define MIN(a,b)((a) <= (b) ? (a) : (b))
#define MAX(a,b)((a) >= (b) ? (a) : (b))
#define __MY_FILE__ ((strrchr(__FILE__, '/') == NULL) ? __FILE__ : strrchr(__FILE__, '/') + 1)
#define STR(x)			#x

#define UID_LENGTH			 (64)    // 帐号长度
#define NAME_LENGTH          (32)    // 名字长度
#define PATH_LENGTH			 (256)    // 文件路径字符长度
#define ADDR_LENGTH          (32)    // IP和PORT的长度
#define GUIDE_MAX            (100)
#define MAX_BROADCAST_NUM    (1000)            // 单词最大广播数量

#define	TCP_BACK_LOG		 (1024)
#define RECVBUFLENGTH        (1024*1024*6)        // 接收缓冲区大小
#define POSTBUFLENGTH        (1024*1024*6)        // 发送缓冲区大小
#define RECV_BUF_LEN         (8 * 1024)   // 接收客户端信息的缓冲区
#define SECOND_ABOVE_CONVERSION_UNIT    60                // 秒以上换算单位
#define SECOND_UNDER_CONVERSION_UNIT    1000            // 秒以下换算单位
#define MSG_HEAD_LEN 8  // 接收或发送给客户端消息的消息头字节数
#define MSG_MAX_LEN                10*1024            // 接收或发送给客户端消息的最大字节数


#define DELETE(ptr) \
	do \
	{\
		if(ptr != NULL)\
		{\
			delete ptr;\
			ptr = NULL;\
		}\
	}\
	while(0);\

#define DELETE_ARR(ptr) \
	do \
	{\
		if(ptr != NULL)\
		{\
			delete[] ptr;\
			ptr = NULL;\
		}\
	}\
	while(0);\

typedef unsigned char BYTE;
typedef int int32;
typedef short int16;
typedef char int8;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef False
#define False  0
#endif

#ifndef True
#define True   1
#endif

typedef std::vector<std::string> Tokens;

Tokens StrSplit(const std::string &src, const std::string &sep);

void ignore_pipe();

// 分割字符串，获取单词
void TrimStr(char *strInput);

//// 将sockaddr_in中的ip和port转换成string
//int SockAddrToString(sockaddr_in *pstSockAddr, char *szResult);
//// 将ip和port转化成string
//int SockAddrToString(unsigned int ip, unsigned short port, char *szResult);

//// 获取两段时间的间隔
//int TimeValMinus(timeval &tvA, timeval &tvB, timeval &tvResult);

// 广播列表，为了速度考虑，用数组实现
struct stPointList
{
	void *mPointList[MAX_BROADCAST_NUM];    // 保存需要广播的玩家的FD
	int mPointNum;                            // 广播数量

	void Clear()
	{
		mPointNum = 0;
	}

	stPointList()
	{
		Clear();
	}

	int push_back(void *pPoint)
	{
		if (mPointNum >= MAX_BROADCAST_NUM)
			return -1;
		mPointList[mPointNum] = pPoint;
		return mPointNum++;
	}

	void *GetPointByIdx(int iIdx)
	{
		if (iIdx < 0 || iIdx >= mPointNum || iIdx >= MAX_BROADCAST_NUM)
			return NULL;
		return mPointList[iIdx];
	}

	int GetBroadcastNum()
	{
		return mPointNum;
	}
};

#endif // __BASE_H__
