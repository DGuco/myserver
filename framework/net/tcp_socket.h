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

enum eTcpStatus
{
	eTcpClosed = 0,
	eTcpCreated = 1,
	eTcpConnecting = 2,
	eTcpConnected = 3,
	eTcpRegisting = 4,
	eTcpRegistered = 5,
};

class CTCPSocket
{
public:
	//
	CTCPSocket(unsigned int RecvBufLen_, unsigned int SendBufLen_);
	//
	CTCPSocket(CSocket socket,unsigned int RecvBufLen_, unsigned int SendBufLen_);
	//
	virtual ~CTCPSocket();
	//获取socketid
	SOCKET GetSocketFD();
	//socket 
	CSocket& GetSocket();
	//获取连接状态
	eTcpStatus GetStatus();
	//连接
	int ConnectTo(const char* szIPAddr, u_short unPort,bool block = true);
	//检查非阻塞连接是否连接成功
	int CheckConnectedOk();
	//读取数据
	int Recv();
	//把数据写到缓冲区准备发送
	//sendnow 是否立刻发送到网络中，1 立刻发送 2 不发送只暂存到缓冲区中等tcp发送缓冲区可写是再一起发出去
	int Write(BYTE* pCode, msize_t nCodeLength,bool sendnow = false);
	//把缓冲区数据发送
	int Flush();
	//添加socket到fdset
	bool AddToFDSet(fd_set& pCheckSet);
	//是否添加到fdset中
	bool IsFDSetted(fd_set& pCheckSet);
	//关闭
	int Close();
	//
	CSafePtr<CByteBuff> GetReadBuff();
	//
	CSafePtr<CByteBuff> GetSendBuff();
protected:
	CSocket					m_Socket;	     //Socket 描述符
	int						m_nStatus;	     //连接状态
	int						m_nRecvBuffLen;  //接受缓冲区大小
	int						m_nSendBuffLen;  //发送缓冲区大小
	CSafePtr<CByteBuff>	m_pReadBuff;     //读缓冲
	CSafePtr<CByteBuff>	m_pWriteBuff;    //写缓冲
};
#endif //__TCP_SOCKET_H__
