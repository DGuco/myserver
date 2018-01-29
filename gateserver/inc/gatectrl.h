//
//  tcpctrl.h
//  客户端tcp管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef __GATE_CTRL_H__
#define __GATE_CTRL_H__

#include <sys/epoll.h>
#include "gatedef.h"
#include "log.h"
#include "servertool.h"
#include "message.pb.h"
#include "base.h"
#include "config.h"
#include "net_work.h"

#define MAX_ERRNO_NUM 10
#define READSTAT      0
#define WRITESTAT     1

class CCodeQueue;
class CSharedMem;
class CGateCtrl
{
public:
	CGateCtrl();
	~CGateCtrl();
	int Initialize();        // 初始化
	int Run();
	int SetRunFlag(int iRunFlag);
	int CreatePipe();

private:
	//*****************Epoll***********************/
	//初始化socket
	bool BeginListen();
	//******************数据处理**********************/
	//接收客户端上行消息
	int RecvClientData(int iSocketFd);
	// 检测客户端socket是否超时
	int CheckTimeOut();
	//读取tcp数据
	int TcpRead(int iSocket, char *pBuf, int iLen);
	//发送数据
	int TcpWrite(int iSocket, char *pBuf, int iPackLen);
	//清除socket
	void ClearSocketInfo(short enError);

	//通知game玩家断开连接
	void DisConnect(int iError);
	//接收gate数据
	int RecvServerData();
	//检测发送队列
	int CheckWaitSendData();
	//向client下行数据
	int SendClientData();

protected:
	static void OnAcceptCns(uint32 uId, CAcceptor *pAcceptorEx);
	static void OnCnsDisconnected(CAcceptor *pAcceptorEx);
	static void OnCnsSomeDataSend(CAcceptor *pAcceptorEx);
	static void OnCnsSomeDataRecv(CAcceptor *pAcceptorEx);

private:
	int m_iRunFlag;
	time_t m_iLastTime;                             // 上次检测超时的时间
	time_t m_iNowTime;                              // 当前时间
	int m_iSocket;                                  // 监听socket
	struct sockaddr_in m_stSockAddr;                        // 网络地址

	int m_iTimeout;
	TTcpStat m_stTcpStat;                           // 当前tcp连接信息

	char m_szCSMsgBuf[MAX_PACKAGE_LEN];             // 发送客户端上行消息给gameserver缓冲
	char m_szSCMsgBuf[MAX_PACKAGE_LEN];             // 下行客户端发送消息缓冲区
	unsigned short m_iSCIndex;                      // 去掉nethead头的实际发送给客户端的数据在m_szSCMsgBuf中的数组下标
	short m_nSCLength;                              // 实际发送的数据长度

	MesHead m_S2CHead;
	// tcp --> game通信共享内存管道
	CCodeQueue *mC2SPipe;
	// game --> tcp通信共享内存管道
	CCodeQueue *mS2CPipe;
	int m_iSendIndex;                               // 带要发送数据过去的client socket索引
	bool m_bHasRecv;                                // 是否接收过数据(只用于第一次接收数据使用,防止没有RecvData直接GetOneCode报错)
	CNetWork *m_pNetWork;
};

#endif
