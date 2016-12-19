//
//  commdef.cpp
//  客户端tcp管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef __TCPCTRL_H__
#define __TCPCTRL_H__

#include <sys/epoll.h>
#include "../../common/tools/inc/tcp_conn.h"
#include "../../common/tools/inc/log.h"
#include "../../common/tools/inc/servertool.h"
#include "../../common/message/tcpmessage.pb.h"
#include "../../common/message/message.pb.h"
#include "../../common/tools/inc/protoconfig.h"
#include "../inc/commdef.h"

#define MAX_ERRNO_NUM 10
#define READSTAT      0
#define WRITESTAT     1

class CTcpCtrl
{
public:
    CTcpCtrl();
    ~CTcpCtrl();
    int Initialize();		// 初始化
    int Run();
    int SetRunFlag(int iRunFlag);

public:

    //*****************Epoll***********************/
    int InitEpollSocket(short shPort);
    int EphInit();
    int EphSocket(int iDomain,int iType,int iProtocol);
    int EphNewConn(int iSocketFd);
    int EphClose(int iSocketFd);
    int EphCleanUp();

    //******************数据处理**********************/
    int GetExMessage();
    int RecvClientData(int iSocketFd);
    int TcpRead(int iSocket, char *pBuf, int iLen);
    void ClearSocketInfo(short enError);

private:

    static BYTE*        pCurrentShm;                     	// 内存块的指针     // 配置信息
    int                 miRunFlag;
    time_t              miLastTime;                     	// 上次检测超时的时间
    time_t              miLastWriteStatTime;            	// 上次检测超时的时间
    time_t              miNowTime;                      	// 当前时间
    struct  timeval     mtvWait;                        	// 网络事件超时设置
    int 				miSocket;                       	// 监听socket
    struct sockaddr_in  mstSockAddr;                    	// 网络地址
    TSocketInfo 		mastSocketInfo[MAX_SOCKET_NUM]; 	// socket结构数组，用于记录客户端的信息
    TSocketInfo* 		mpSocketInfo;                   	// 当前的socket结构指针

    char                mszCSPipeFile[100];             	// 本地文件
    char                mszSCPipeFile[100];             	// 本地文件

    char                mszMsgBuf[MAX_BUF_LEN]; 		 	// 消息包缓冲(加大是为了防止game过来的消息过大)
    int                 miTimeout;
    TTcpStat            mstTcpStat;                         // 当前tcp连接信息
    int                 miWriteStatCount;
    char                mszWriteStatBuf[1024];
    struct epoll_event* mpEpollevents;                      //epoll event集合(大小MAX_SOCKET_NUM)
    int                 miKdpfd;                            //epoll描述符
    int                 miMaxfds;
    struct epoll_event  mstEpollEvent;

    unsigned char 		mszSCMsgBuf[MAX_BUF_LEN]; 		 	// 发送消息缓冲区
    unsigned short 		miSCIndex; 					 	    // 去掉nethead头的实际发送给客户端的数据在m_szSCMsgBuf中的数组下标
    short 				mnSCLength; 					 	// 实际发送的数据长度

//    CGateClient		mGateClient;
    time_t				mLastKeepaliveTime;
    CTcpHead			mSCTcpHead;				// 服务器发送到客户端的数据信息头
    int					miSendIndex;				// 已发送的服务器到客户端的数据的游标
    bool				mbHasRecv;					// 是否接收过数据(只用于第一次接收数据使用,防止没有RecvData直接GetOneCode报错)
};

#endif
