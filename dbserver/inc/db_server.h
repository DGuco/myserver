/*****************************************************************
* FileName:db_server.h
* Summary :
* Date	  :2024-6-13
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __DB_SERVER_H__
#define __DB_SERVER_H__

#include "singleton.h"
class CDBSerer : public CSingleton<CDBSerer>
{
public:
	// 运行标志
	enum ERunFlag
	{
		EFLG_CTRL_NO = 0,
		EFLG_CTRL_QUIT = 1,
		EFLG_CTRL_SHUTDOWN = 2,
		EFLG_CTRL_RELOAD = 3
	};
public:
	//构造函数
	CDBSerer();
	//析构函数
	~CDBSerer();
	//准备启动
	int PrepareToRun();
	//启动
	int Run();
	//设置运行标志
	void SetRunFlag(int iFlag);
	//清除运行标志
	void ClearRunFlag(int iFlag);
	//运行标志是否设置
	bool IsRunFlagSet(int iFlag);
	//转发笑傲析
	int SendMessageTo(CProxyMessage* pMsg);
	//处理数据
	int Event(CProxyMessage* pMsg);
	//处理sql
	int ProcessExecuteSqlRequest(CProxyMessage* pMsg);
	//释放结果
	void ReleaseResult(QueryResult* res)
	{
		if (res != NULL) {
			delete res;
			res = NULL;
		}
	}
private:
	//连接proxy
	int ConnectToProxyServer();
	//向proxy注册
	int RegisterToProxyServer(CConnector* pConnector);
	//向proxy发送心跳消息
	int SendkeepAliveToProxy(CConnector* pConnector);
	//分发消息
	int DispatchOneCode(int nCodeLength, CByteBuff* pbyCode);
	//获取收到心跳的时间
	time_t GetLastSendKeepAlive() const;
	//获取上次发送心跳的时间
	time_t GetLastRecvKeepAlive() const;
	//设置上次发送心跳的时间
	void SetLastSendKeepAlive(time_t tLastSendKeepAlive);
	//设置上次收到心跳的时间
	void SetLastRecvKeepAlive(time_t tLastRecvKeepAlive);
private:
	//连接成功回调
	static void lcb_OnConnected(IBufferEvent* pBufferEvent);
	//断开连接回调
	static void lcb_OnCnsDisconnected(IBufferEvent* pBufferEvent);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(IBufferEvent* pBufferEvent);
	//连接失败回调(无用)
	static void lcb_OnConnectFailed(IBufferEvent* pBufferEvent);
	//发送数据回调(无用)
	static void lcb_OnCnsSomeDataSend(IBufferEvent* pBufferEvent);
	//发送心跳倒计时回调
	static void lcb_OnPingServer(int fd, short event, CConnector* pConnector);
	static void lcb_OnSigPipe(uint, void*);

public:
	static CByteBuff m_acRecvBuff;
	static int m_iProxyId;
private:
	int m_iRunFlag;    // 运行标志
	CSafePtr<Database> m_pDatabase;
	//dbserver ==> gameserver
	CSafePtr<CShmMessQueue>	m_DB2SCodeQueue;
	//gameserver ==> dbserver
	CSafePtr<CShmMessQueue>	m_S2DBCodeQueue;
};
#endif //__DB_SERVER_H__
