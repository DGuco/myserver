//
//  socket.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
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
	bool Bind(std::string host,int port);
	//监听
	bool Listen();
	//获取远程ip
	bool GetRemoteAddress(CNetAddr & addr) const;
	//获取socket fd
	SOCKET GetSocket() const;
private:
	SOCKET  m_Socket;
};

#endif
