//
//  tcp_conn.h
//  tcp管理
//  Created by DGuco on 17/04/24.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _TCP_CONN_H_
#define _TCP_CONN_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "../base/base.h"
#include "../log/log.h"
#include "tcp_conn.cpp"
#ifdef _POSIX_MT_
#include <mutex>
#endif

#define  SENDBUFSIZE 300000
//以下定义tcp连接状态
enum eTCPConnStates
{
	tcs_closed = 0,
	tcs_opened,
	tcs_connected,
	tcs_error,
	tcs_connecting,
};
//以下定义Socket类型
enum eSockTypes
{
	sot_listen = 0,
	sot_conn
};
//以下定义接受/发送错误类型
enum eRecvErrs
{
	ERR_RECV_NOSOCK = -1,
	ERR_RECV_NOBUFF = -2,
	ERR_RECV_REMOTE = -3,
	ERR_RECV_FALIED = -4
};
enum eSendErrs
{
	ERR_SEND_NOSOCK = -1,
	ERR_SEND_NOBUFF = -2,
	ERR_SEND_FAILED = -3,
	ERR_SEND_UNKOWN = -4,
};

enum eByteMode
{
	use_host_byte = 0,
	use_network_byte = 1
};

enum eLinkMode
{
	em_block_mode = 0,
	em_nblock_mode = 1
};

template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
class CTCPSocket
{
public:
	//构造函数
	CTCPSocket();
	//析构函数
	~CTCPSocket();
	//获取socket描述符
	int GetSocketFD();
	//获取tcp状态
	int GetStatus();
	//设置socket
	inline void SetSocketFD( int nSockFD ) { m_iSocketFD = nSockFD; }
	//设置tcp状态
	inline void SetStatus( int nStatus ) { m_iStatus = nStatus; }
	//创建tcp client
	int CreateClient( char* szLocalAddr = NULL );
	//创建tcp server
	int CreateServer(unsigned short unPort, char* szIPAddr=NULL );
	//连接到指定的ip地址
	int ConnectTo(char *szIPAddr, unsigned short unPort);
	//指定阻塞或者非阻塞方式连接到指定的ip
	int ConnectTo(u_long ulIPNetAddr, u_short unPort, eLinkMode emBlock = em_block_mode);
	//接受socket连接
	int Accept( int iAcceptFD );
	//接受socket数据到接收缓冲去
	int RecvData();
	// 从缓冲区获取指定长度的数据
	int GetOneCode( unsigned short &nCodeLength, BYTE *pCode, eByteMode emByte = use_host_byte);
	int GetOneCode32( int &iCodeLength, BYTE * pCode);
	//从写缓冲区发送指定长度的数据
	int SendOneCode( unsigned short nCodeLength, BYTE *pCode  );
	int SendOneCode32( int nCodeLength, BYTE *pCode  );
	//fd_set socket
	int AddToCheckSet( fd_set *pCheckSet );
	//pCheckSet是否set
	int IsFDSetted( fd_set *pCheckSet );
	//设置非阻塞
	int SetNBlock(int iSock);
	//是否有数据要发送
	int HasReserveData();
	//发送发送缓冲去数据
	int CleanReserveData();
    //打印socket状态
	int PrintfSocketStat();
    //检测非阻塞连接
	int CheckNoblockConnecting(int nto = 0);
    //获取发送和接受缓冲区数据索引
	void GetCriticalData(int& iReadBegin,int& iReadEnd, int& iPostBegin, int& iPostEnd);
    //关闭socket
	int Close();

protected:
	int m_iSocketFD;					//Socket描述子
	int m_iSocketType;					//Socket类型
	int m_iStatus;						//连接状态
#ifdef _POSIX_MT_
	std::mutex m_stMutex;			//操作的互斥变量
#endif

private:
	int  m_iReadBegin;
	int  m_iReadEnd;
	BYTE m_abyRecvBuffer[uiRecvBufLen+1];


	int  m_iPostBegin;
	int  m_iPostEnd;
	BYTE m_abyPostBuffer[uiSendBufLen+1];

};


template<unsigned int uiRecvBufLen, unsigned int uiSendBufLen>
class CTCPConn
{
public:
	CTCPConn();
	~CTCPConn();

	int Initialize(short nEntityType, short nEntityID, u_long ulIPAddr, u_short unPort);
	int ConnectToServer(char* szLocalAddr  = NULL, eLinkMode emBlock = em_block_mode);
	int CreateServer();
	int GetEntityInfo(short* pnEntityType, short* pnEntityID, unsigned long* pulIpAddr);
	u_long GetConnAddr();
	u_short GetConnPort();
	short  GetEntityType();
	short  GetEntityID();
	CTCPSocket<uiRecvBufLen, uiSendBufLen>* GetSocket();

private:
	short   m_nEntityType;
	short   m_nEntityID;
	CTCPSocket<uiRecvBufLen, uiSendBufLen> m_stSocket;
	u_long m_ulIPAddr;
	u_short m_unPort;
};
#endif // _TCP_CONN_H_

