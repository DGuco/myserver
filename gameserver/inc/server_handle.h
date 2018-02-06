//
// Created by dguco on 18-2-5.
// 服务器之间连接管理
//

#ifndef SERVER_SERVER_HANDLE_H
#define SERVER_SERVER_HANDLE_H
#include <net_work.h>

class CServerHandle
{
public:
	CServerHandle();
	~CServerHandle();
	// 运行准备
	int PrepareToRun();
	// 运行
	void Run();
	// 连接到Proxy
	bool Connect2Proxy();
	// 向Proxy注册
	bool Regist2Proxy();
	// 发送心跳到Proxy
	bool SendKeepAlive2Proxy();
	// 向dbserver发送数据
	void SendMessageToDB(char *data, PACK_LEN len);
private:
	void SendMessageToProxy(char *data, PACK_LEN len);
private:
	static void lcb_OnConnectted(CConnector *pConnector);
	//断开连接回调
	static void lcb_OnCnsDisconnected(CConnector *pConnector);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(CConnector *pConnector);
	static void lcb_OnConnectFailed(CConnector *pConnector);
	static void lcb_OnCnsSomeDataSend(CConnector *pConnector);
	static void lcb_OnPingServer(CConnector *pConnector);
protected:
	static void SetProxyId(int id);
	static int GetProxyId();
private:
	CNetWork *m_pNetWork;                // 服务器间通信的连接
private:
	static int m_iProxyId;
	static char m_acRecvBuff[MAX_PACKAGE_LEN];
};


#endif //SERVER_SERVER_HANDLE_H
