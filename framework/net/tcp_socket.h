/*****************************************************************
* FileName:tcp_client.h
* Summary :
* Date	  :2023-8-11
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
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
	virtual ~CTCPSocket();
	//获取socketid
	SOCKET GetSocketFD();
	//获取连接状态
	eTcpStatus GetStatus();
	//连接
	int ConnectTo(const char* szIPAddr, u_short unPort,bool block = true);
	//连接
	int ConnectTo(u_long ulIPNetAddr, u_short unPort,bool block = true);
	//检查非阻塞连接是否连接成功
	int CheckConnectedOk();
	//读取数据
	int RecvData();
	//把数据写到缓冲区准备发送
	int Write(BYTE* pCode, msize_t nCodeLength);
	//获取读缓冲区中的一段信息
	int GetOneCode(unsigned short& nCodeLength, BYTE* pCode);
	//添加socket到fdset
	bool AddToFDSet(fd_set& pCheckSet);
	//是否添加到fdset中
	bool IsFDSetted(fd_set& pCheckSet);
	//是否连接成功
	int CheckConnectedOk();
	//Init tcp Server
	int InitTcpServer(const char* ip,int port);
	//关闭
	int Close();
public:
	//是否可读写
	virtual bool CanReadWrite() { return 1;};
protected:
	CSocket					m_Socket;	     //Socket 描述符
	int						m_nStatus;	     //连接状态
	int						m_nRecvBuffLen;  //接受缓冲区大小
	int						m_nSendBuffLen;  //发送缓冲区大小
	SafePointer<CByteBuff>	m_pReadBuff;     //读缓冲
	SafePointer<CByteBuff>	m_pWriteBuff;    //写缓冲
};
#endif //__TCP_CLIENT_H__
