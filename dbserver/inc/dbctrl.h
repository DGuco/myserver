//
// Created by DGuco on 17-7-13.
//

#ifndef _DBCTRL_HPP_
#define _DBCTRL_HPP_

#include "dbhandle.h"
#include "base_db.h"

#include "configure.pb.h"
//#include "protoconfig.h"

#ifdef _ASYNC_SQL_
#include "asqlexe.h"
#endif

#define MAXPROXYNUMBER				2					// 最大game server 数目

#define MAXHANDLENUMBER				2

#define SECSOFONEHOUR				3600

#define CHECKINTERVAL				10					// 检查时间间隔，单位：秒
#define PROXYKEEPALIVESECONDS		(3*CHECKINTERVAL)	// 与proxy的心跳超时时间
#define MAXPROXYCODELEN				1024				// 向proxy发送纯命令的最大块长度
#define STATISTICSLEN				1024
#define MAXNUMBERLEN				10
#define SAVEINTERVAL				300


class CSharedMem;


class CDBCtrl : public CSingleton< CDBCtrl >
{
public:
    CDBCtrl();
    ~CDBCtrl();

    int Initialize( );
    int PrepareToRun();
    int Run();

    static CSharedMem *mShmPtr;
	// 运行标志
	enum ERunFlag
	{
		EFLG_CTRL_NO		= 0,
		EFLG_CTRL_QUIT		= 1,
		EFLG_CTRL_SHUTDOWN	= 2,
		EFLG_CTRL_RELOAD	= 3 
	};

	void SetRunFlag( int iFlag );
	void ClearRunFlag( int iFlag);
	bool IsRunFlagSet( int iFlag );

private:

    int	ConnectToProxyServer();
    int RegisterToProxyServer(int nIndex);
    int SendkeepAliveToProxy(int nIndex);     // 向proxy发送心跳消息
    int CheckRunFlags();
    int CheckAndDispatchInputMsg();
    int RoutineCheck();
    int DispatchOneCode(int iProxyIdx, int nCodeLength, BYTE* pbyCode, bool vCountNum = true);
    int PostInternalMsgToHandle(int iHandleID, CMessage *pMsg);
    int NotifyHandleClearComplete();
	int GetThisRoundHandle();
private:
	
	int m_iRunFlag;	// 运行标志
    CTCPConn<RECVBUFLENGTH, POSTBUFLENGTH>		m_astProxySvrdCon[MAXPROXYNUMBER];
    CDBHandle *	m_apHandles[MAXHANDLENUMBER];
	int		m_which_handle;								// 收到消息放到哪个线程
    time_t	m_atLastSendKeepAlive[MAXPROXYNUMBER];		// 最后发送proxy心跳消息时间
    time_t	m_atLastRecvKeepAlive[MAXPROXYNUMBER];		// 最后接收proxy心跳消息时间
	time_t	m_lastTick;
};

#endif


