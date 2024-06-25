//
//  socket.h
//  Created by DGuco on 18/01/20.
//  Copyright ? 2018�� DGuco. All rights reserved.
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
	//���캯��
	CSocket();
	//��������
	~CSocket();
	//��
	bool Open(int nProtocolFamily = AF_INET, int nType = SOCK_STREAM, int nProtocol = 0);
	//�ر�
	void Close();
	//shutdown
	void Shutdown();
	//shutdown read
	void ShutdownRead();
	//shutdown write
	void ShutdownWrite();
	//�󶨶˿�
	bool Bind(int port);
	//�󶨶˿�
	bool Bind(const char* ipaddr,int port);
	//conn
	int Conn(const char* ipaddr, int port,bool block);
	//����
	bool Listen();
	//accept
	CSocket Accept();
	//��ȡ
	int  Read(char* data,int len);
	//д��
	int  Write(char* data, int len);
	//��ȡԶ��ip
	bool GetRemoteAddress(CNetAddr & addr) const;
	//���÷��ͻ�������С
	bool SetSendBufSize(int size);
	//��ȡ���ͻ�������С
	int GetSendBuffSize();
	//���ý��ջ�������С
	bool SetRecvBufSize(int size);
	//��ȡ���ջ�������С
	int GetRecvBuffSize();
	//��ȡsocket fd
	SOCKET GetSocket() const;
	//set opt
	int SetSocketOpt(int sol, int type,const void* value, int size);
	//get opt
	int GetSocketOpt(int sol, int type,void* value, int* size);
	//���÷�����
	bool SetSocketNoBlock();
	//socket ����
	bool SetReuseAddr();
	//�Ƿ�����
	bool IsReuseAddr();
	//set Linger
	bool SetLinger(int lingertime);
	//get Linger
	int GetLinger();
	//tcp ����
	bool SetKeepAlive();
	///tcp ����
	bool IsKeepAlive();
	//tcp NoDelay
	bool SetTcpNoDelay();
	//tcp NoDelay
	bool IsTcpNoDelay();
	//�ļ����������ж������ݿ��Զ�ȡ
	unsigned int CanReadLen();
	//�Ƿ���Ч
	bool IsValid();
	//socket �Ƿ����
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
