//
//  tcpctrl.h
//  客户端tcp管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef __TCPCTRL_H__
#define __TCPCTRL_H__

#include <sys/epoll.h>
#include "../../framework/net/tcp_conn.h"
#include "../../framework/log/log.h"
#include "../../framework/base/servertool.h"
#include "../../framework/message/tcpmessage.pb.h"
#include "../../framework/message/message.pb.h"
#include "../../framework/base/base.h"
#include "tcpdef.h"
#include "../../framework/json/config.h"

#define MAX_ERRNO_NUM 10
#define READSTAT      0
#define WRITESTAT     1

//CServerConfig* g_pConfigJson = NULL;

class CCodeQueue;
class CSharedMem;

typedef CTCPConn<RECVBUFLENGTH,POSTBUFLENGTH> MyTcpConn;

class CTcpCtrl
{
public:
    CTcpCtrl();
    ~CTcpCtrl();
    int Initialize();		// 初始化
    int Run();
    int SetRunFlag(int iRunFlag);
    int CreatePipe();

private:

    //*****************Epoll***********************/
    //初始化socket
    int InitEpollSocket(short shPort);
    //初始化epoll
    int EphInit();
    //设置epoll socket
    int EphSocket(int iDomain,int iType,int iProtocol);
    //添加新的连接
    int EphNewConn(int iSocketFd);
    //关闭epoll
    int EphClose(int iSocketFd);
    //清除epoll
    int EphCleanUp();

    //******************数据处理**********************/
    //检测epoll事件消息
    int GetExMessage();
    //接收客户端上行消息
    int RecvClientData(int iSocketFd);
    //读取tcp数据
    int TcpRead(int iSocket, char *pBuf, int iLen);
    //发送数据
    int TcpWrite(int iSocket, char *pBuf, int iPackLen);
    //清除socket
    void ClearSocketInfo(short enError);

    //通知game玩家断开连接
    void DisConnect(int iError);
    //接收gate数据
    int  RecvServerData();
    //检测发送队列
    int CheckWaitSendData();
    //向client下行数据
    int SendClientData();

private:

    static BYTE*        m_pCurrentShm;                     	// 内存块的指针     // 配置信息
    int                 m_iRunFlag;
    time_t              m_iLastTime;                     	// 上次检测超时的时间
    time_t              m_iLastWriteStatTime;            	// 上次检测超时的时间
    time_t              m_iNowTime;                      	// 当前时间
    struct  timeval     m_tvWait;                        	// 网络事件超时设置
    int 				m_iSocket;                       	// 监听socket
    struct sockaddr_in  m_stSockAddr;                    	// 网络地址
    TSocketInfo 		m_astSocketInfo[MAX_SOCKET_NUM]; 	// socket结构数组，用于记录客户端的信息
    TSocketInfo* 		m_pSocketInfo;                   	// 当前的socket结构指针

    char                m_szCSPipeFile[100];             	// 本地文件
    char                m_szSCPipeFile[100];             	// 本地文件

    char                m_szMsgBuf[MAX_BUF_LEN]; 		 	// 消息包缓冲(加大是为了防止game过来的消息过大)
    int                 m_iTimeout;
    TTcpStat            m_stTcpStat;                        // 当前tcp连接信息
    int                 m_iWriteStatCount;
    char                m_szWriteStatBuf[1024];
    struct epoll_event* m_pEpollevents;                      //epoll event集合(大小MAX_SOCKET_NUM)
    int                 m_iKdpfd;                            //epoll描述符
    int                 m_iMaxfds;
    struct epoll_event  m_stEpollEvent;

    unsigned char 		m_szSCMsgBuf[MAX_BUF_LEN]; 		 	 // 发送消息缓冲区
    unsigned short 		m_iSCIndex; 					 	 // 去掉nethead头的实际发送给客户端的数据在m_szSCMsgBuf中的数组下标
    short 				m_nSCLength; 					 	 // 实际发送的数据长度

    // tcp --> game通信共享内存管道
    CCodeQueue* mC2SPipe;
    // game --> tcp通信共享内存管道
    CCodeQueue* mS2CPipe;
    time_t				m_iLastKeepaliveTime;
    CTcpHead			m_SCTcpHead;				        // 服务器发送到客户端的数据信息头
    int					m_iSendIndex;				        // 带要发送数据过去的client socket索引
    bool				m_bHasRecv;					        // 是否接收过数据(只用于第一次接收数据使用,防止没有RecvData直接GetOneCode报错)
};

#endif
