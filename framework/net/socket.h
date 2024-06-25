//
//  socket.h
//  Created by DGuco on 18/01/20.
//  Copyright ? 2018年 DGuco. All rights reserved.
//

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <string>
#include "t_array.h"
#include "base.h"

using namespace my_std;

struct CNetAddr
{
	CString<16> m_szAddr;
	unsigned int m_uPort;
};

class CSocket
{
public:
	//构造函数
	CSocket();
	//析构函数
	~CSocket();
	//打开
	bool Open(int nProtocolFamily = AF_INET, int nType = SOCK_STREAM, int nProtocol = 0);
	//关闭
	void Close();
	//shutdown
	void Shutdown();
	//shutdown read
	void ShutdownRead();
	//shutdown write
	void ShutdownWrite();
	//绑定端口
	bool Bind(int port);
	//绑定端口
	bool Bind(const char* ipaddr,int port);
	//conn
	int Conn(const char* ipaddr, int port,bool block);
	//监听
	bool Listen();
	//accept
	CSocket Accept();
	//读取
	int  Read(char* data,int len);
	//写入
	int  Write(char* data, int len);
	//获取远程ip
	bool GetRemoteAddress(CNetAddr & addr) const;
	//设置发送缓冲区大小
	bool SetSendBufSize(int size);
	//获取发送缓冲区大小
	int GetSendBuffSize();
	//设置接收缓冲区大小
	bool SetRecvBufSize(int size);
	//获取接收缓冲区大小
	int GetRecvBuffSize();
	//获取socket fd
	SOCKET GetSocket() const;
	//set opt
	int SetSocketOpt(int sol, int type,const void* value, int size);
	//get opt
	int GetSocketOpt(int sol, int type,void* value, int* size);
	//设置非阻塞
	bool SetSocketNoBlock();
	//socket 重用
	bool SetReuseAddr();
	//是否重用
	bool IsReuseAddr();
	//set Linger
	bool SetLinger(int lingertime);
	//get Linger
	int GetLinger();
	//tcp 心跳
	bool SetKeepAlive();
	///tcp 心跳
	bool IsKeepAlive();
	//tcp NoDelay
	bool SetTcpNoDelay();
	//tcp NoDelay
	bool IsTcpNoDelay();
	//文件描述符中有多少数据可以读取
	unsigned int CanReadLen();
	//是否有效
	bool IsValid();
	//socket 是否出错
	bool IsSocketError();
	// host 
	const CString<ADDR_LENGTH>& GetHost() const;
	//port
	int	GetPort() const;
private:
	sockaddr_in				m_SocketAddr;
	CString<ADDR_LENGTH>	m_Host;
	int						m_nPort;
	SOCKET					m_nSocket;
};

#endif
