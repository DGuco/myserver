//
//  base.h
//  ��Ϸ����
//  Created by DGuco on 16/12/19.
//  Copyright ? 2016�� DGuco. All rights reserved.
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

#define UID_LENGTH			 (64)    // �ʺų���
#define NAME_LENGTH          (32)    // ���ֳ���
#define PATH_LENGTH			 (256)    // �ļ�·���ַ�����
#define ADDR_LENGTH          (32)    // IP��PORT�ĳ���
#define GUIDE_MAX            (100)
#define MAX_BROADCAST_NUM    (1000)            // �������㲥����

#define	TCP_BACK_LOG		 (1024)
#define RECVBUFLENGTH        (1024*1024*6)        // ���ջ�������С
#define POSTBUFLENGTH        (1024*1024*6)        // ���ͻ�������С
#define RECV_BUF_LEN         (8 * 1024)   // ���տͻ�����Ϣ�Ļ�����
#define SECOND_ABOVE_CONVERSION_UNIT    60                // �����ϻ��㵥λ
#define SECOND_UNDER_CONVERSION_UNIT    1000            // �����»��㵥λ
#define MSG_HEAD_LEN 8  // ���ջ��͸��ͻ�����Ϣ����Ϣͷ�ֽ���
#define MSG_MAX_LEN                10*1024            // ���ջ��͸��ͻ�����Ϣ������ֽ���


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

// �ָ��ַ�������ȡ����
void TrimStr(char *strInput);

//// ��sockaddr_in�е�ip��portת����string
//int SockAddrToString(sockaddr_in *pstSockAddr, char *szResult);
//// ��ip��portת����string
//int SockAddrToString(unsigned int ip, unsigned short port, char *szResult);

//// ��ȡ����ʱ��ļ��
//int TimeValMinus(timeval &tvA, timeval &tvB, timeval &tvResult);

// �㲥�б�Ϊ���ٶȿ��ǣ�������ʵ��
struct stPointList
{
	void *mPointList[MAX_BROADCAST_NUM];    // ������Ҫ�㲥����ҵ�FD
	int mPointNum;                            // �㲥����

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
