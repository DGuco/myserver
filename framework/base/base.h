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
#include "my_new.h"

#define MIN(a,b)((a) <= (b) ? (a) : (b))
#define MAX(a,b)((a) >= (b) ? (a) : (b))
#define __MY_FILE__ ((strrchr(__FILE__, '/') == NULL) ? __FILE__ : strrchr(__FILE__, '/') + 1)
#define STR(x)			#x

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

#define new MY_NEW
#define SAFE_DELETE(ptr) \
	do \
	{\
		if(ptr != NULL)\
		{\
			delete ptr;\
			ptr = NULL;\
		}\
	}\
	while(0);\

#define SAFE_DELETE_ARR(ptr) \
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
typedef unsigned char byte;
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


#endif // __BASE_H__
