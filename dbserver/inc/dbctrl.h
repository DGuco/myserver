//
// Created by DGuco on 17-7-13.
//

#ifndef _DBCTRL_HPP_
#define _DBCTRL_HPP_

#include "dbhandle.h"
#include "net_work.h"
#include "thread_pool.h"

//#include "protoconfig.h"

#ifdef _ASYNC_SQL_
#include "asqlexe.h"
#endif

#define MAXPROXYNUMBER                2                    // 最大proxy server 数目

#define MAXHANDLENUMBER                2

#define SECSOFONEHOUR                3600

#define CHECKINTERVAL                10                    // 检查时间间隔，单位：秒
#define PROXYKEEPALIVESECONDS        (3*CHECKINTERVAL)    // 与proxy的心跳超时时间
#define MAXPROXYCODELEN                1024                // 向proxy发送纯命令的最大块长度
#define STATISTICSLEN                1024
#define MAXNUMBERLEN                10
#define SAVEINTERVAL                300


class CSharedMem;

class CDBCtrl: public CSingleton<CDBCtrl>
{
public:
	CDBCtrl();
	~CDBCtrl();

	int Initialize();
	int PrepareToRun();
	int Run();

	//创建共享内
	static int MallocShareMem();
	static CSharedMem *mShmPtr;

	// 运行标志
	enum ERunFlag
	{
		EFLG_CTRL_NO = 0,
		EFLG_CTRL_QUIT = 1,
		EFLG_CTRL_SHUTDOWN = 2,
		EFLG_CTRL_RELOAD = 3
	};

	void SetRunFlag(int iFlag);
	void ClearRunFlag(int iFlag);
	bool IsRunFlagSet(int iFlag);

private:

	int ConnectToProxyServer();
	int RegisterToProxyServer();
	int SendkeepAliveToProxy();     // 向proxy发送心跳消息
	int CheckRunFlags();
	int RoutineCheck();
	int DispatchOneCode(int nCodeLength, BYTE *pbyCode);
	int GetThisRoundHandle();
private:
	static void lcb_OnConnected(CConnector *pConnector);
	//断开连接回调
	static void lcb_OnCnsDisconnected(CConnector *pConnector);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(CConnector *pConnector);
	static void lcb_OnConnectFailed(CConnector *pConnector);
	static void lcb_OnCnsSomeDataSend(CConnector *pConnector);
	static void lcb_OnPingServer(CConnector *pConnector);
private:
	int m_iRunFlag;    // 运行标志
	time_t m_tLastSendKeepAlive;        // 最后发送proxy心跳消息时间
	time_t m_tLastRecvKeepAlive;        // 最后接收proxy心跳消息时间
	time_t m_lastTick;
	CNetWork *m_pNetWork;
private:
	static char m_acRecvBuff[MAX_PACKAGE_LEN];
	static int m_iProxyId;
};

#endif


