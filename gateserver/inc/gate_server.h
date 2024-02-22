/*****************************************************************
* FileName:gate_server.h
* Summary :
* Date	  :2024-2-22
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __GATE_SERVER_H__
#define __GATE_SERVER_H__
#include "tcp_server.h"
#include "server_tool.h"
#include "message.pb.h"

class CGamePlayer;
class CGateServer : public CTCPServer,public CSingleton<CGateServer>
{
public:
	//构造函数
	CGateServer();
	//析构函数
	virtual ~CGateServer();
public:
	//准备run
	bool PrepareToRun();
	//发送数据给gameserver
	void DealClientData(SafePointer<CGamePlayer> pGamePlayer, unsigned short len);
	//给特定client发送数据
	void SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len);
	//清除socket
	void ClearSocket(SafePointer<CGamePlayer> pGamePlayer, short iError);
	//通知gameserver client 断开连接
	void DisConnect(SafePointer<CGamePlayer> pGamePlayer, short iError);
	//接受客户端数据
	void RecvClientData(SafePointer<CGamePlayer> pGamePlayer);
protected:
	//客户端连接还回调
	static void lcb_OnAcceptCns(uint32 uId, IBufferEvent *tmpAcceptor);
	//客户端断开连接回调
	static void lcb_OnCnsDisconnected(IBufferEvent *tmpAcceptor);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *tmpAcceptor);
	//发送数据回调
	static void lcb_OnCnsSomeDataSend(IBufferEvent *tmpAcceptor);
	//检测连接超时
	static void lcb_OnCheckAcceptorTimeOut(int fd, short what, void *param);
    //检测是否又数据要发送
    static void lcb_OnCheckSendMsg(int fd, short what, void *param);

	virtual void OnNewConnect(SafePointer<CTCPConn> pConnn);
	//
	virtual SafePointer<CTCPConn> CreateTcpConn(CSocket tmSocket);
	//
	virtual SafePointer<CTCPClient> CreateTcpClient(CSocket tmSocket);
};

#endif //__GATE_SERVER_H__

