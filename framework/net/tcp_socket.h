/*****************************************************************
* FileName:tcp_client.h
* Summary :
* Date	  :2023-8-11
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__
#include "safe_pointer.h"
#include "byte_buff.h"
#include "socket.h"
#include "time_helper.h"
#include "tcp_def.h"

enum eSocketStatus
{
	eSocketClosed = 0,
	eSocketOpen,
	eSocketConnecting,
	eSocketConnected,
	eSocketClosing,
};

class CTCPSocket
{
public:
	//
	CTCPSocket(int RecvBufLen_,
				int SendBufLen_, 
				int MaxRecvBufLen_,
				int MaxSendBufLen_);
	//
	CTCPSocket(CSocket socket,
				int RecvBufLen_,
				int SendBufLen_,
				int MaxRecvBufLen_,
				int MaxSendBufLen_);
			//
	virtual ~CTCPSocket();
	//获取socketid
	SOCKET GetSocketFD();
	//socket 
	CSocket& GetSocket();
	//获取连接状态
	u_short GetStatus();
	//连接
	int ConnectTo(const char* szIPAddr, u_short unPort,bool block = true);
	//检查非阻塞连接是否连接成功
	int CheckConnectedOk();
	//读取数据
	int Recv();
	//把数据写到缓冲区准备发送
	int Write(BYTE* pCode, int nCodeLength);
	//把缓冲区数据发送
	int Flush();
	//添加socket到fdset
	bool AddToFDSet(fd_set& pCheckSet);
	//是否添加到fdset中
	bool IsFDSetted(fd_set& pCheckSet);
	//关闭
	int Close(bool now = true);
	//
	bool IsValid();
	//
	CSafePtr<CByteBuff> GetReadBuff();
	//
	CSafePtr<CByteBuff> GetSendBuff();
protected:
	CSocket					m_Socket;	     //Socket 描述符
	u_short					m_nStatus;	     //连接状态
	CSafePtr<CByteBuff>		m_pReadBuff;     //读缓冲
	CSafePtr<CByteBuff>		m_pWriteBuff;    //写缓冲
};
#endif //__TCP_SOCKET_H__
