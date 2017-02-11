#ifndef _GATE_CTRL_HPP_
#define _PROXY_CTRL_HPP_

#include "gate_handle.h"
#include "../../framework/base/servertool.h"

#define MAX_TMP_BUF_LEN 		(1024) 				// 注册消息最大长度
#define MAX_UNREGISTER_NUM 		(MAX_CONNS_NUM) 	// 当前未注册的最大个数
#define CHECK_INTERVAL_SECONDS 	(10) 			// 检查时间间隔(秒)
#define CHECK_TIMEOUT_SECONDS 	(30)				// 连接未注册的超时时间(秒)

// 当前连接上，但还未注册结构信息
struct STUnRegisterSocketInfo {
	int 			m_iSocketFD; 		// 连接套接口
	unsigned long 	m_ulIPAddr; 		// 连接ip地址
	time_t  		m_tAcceptTime; 		// 连接发起时间
	int 			m_iRegisted; 		// 是否已经注册(0:未注册 1:已注册)

	STUnRegisterSocketInfo() {
		m_iSocketFD = 0;
		m_ulIPAddr = 0;
		m_tAcceptTime = 0;
		m_iRegisted = 0;
	}

	void Clear() {
		m_iSocketFD = 0;
		m_ulIPAddr = 0;
		m_tAcceptTime = 0;
		m_iRegisted = 0;
	}
};

class CHandleInfo
{
public:
	CDoubleLinkerInfo	mLinkerInfo;	// 链表信息
	CGateHandle*		mpHandle;		// handle
	int					miConnNum;		// 连接计数

	CHandleInfo()
	{
		mpHandle = NULL;
		miConnNum = 0;
	}

	~CHandleInfo()
	{
		if (mpHandle != NULL)
		{
			delete mpHandle;
			mpHandle = NULL;
		}
	}
};


class CGateCtrl {
public:
	CGateCtrl();
	~CGateCtrl();

	enum _enParas
	{
		MUTEX_HASHMAP = 0,

		MUTEX_NUM = 1
	};

#ifdef _POSIX_MT_
	static pthread_mutex_t stLinkMutex[EHandleType_NUM];
	static pthread_mutex_t stMutex[MUTEX_NUM];
#endif

	int Initialize();
	int PrepareToRun();
	int Run();
protected:
private:

	int ReadCfg();
	
	int CheckRunFlags();
	int CheckConnRequest();
	int CheckRoutines();		// 定时检查
	CMyTCPConn* GetConnByAddrAndID(short shSrcID, unsigned long ulSrcAddr);

	int ReceiveAndProcessRegister(int iUnRegisterIdx);	// 接收并处理注册消息
	int DeleteOneUnRegister(int iUnRegisterIdx);		// 删除一个未注册的连接
	
	// 通过KEY获取连接信息
	CMyTCPConn* GetConnByKey(int iKey);
	// 通过FE和ID创建KEY
	int MakeConnKey(const short nType, const short nID);
	// 将连接插入handle并唤醒handle
	int WakeUp2Work(CMyTCPConn* pConn);
	// 获取一个可用连接
	CMyTCPConn* GetCanUseConn();
	// 回收一个连接
	CMyTCPConn* RecycleUnuseConn(CMyTCPConn* pConn, int iIndex = -1);
	// 将连接插入map
	int InsertConnIntoMap(CMyTCPConn* pConn, int iIndex);
	// 将连接从map中移除
	int EraseConnFromMap(CMyTCPConn* pConn, int iIndex);

	CMyTCPSocket m_stListenSocket;

	CMyTCPConn m_astConns[MAX_CONNS_NUM];
	CONNS_MAP m_mapConns;

	CHandleInfo m_stHandleInfos[EHandleType_NUM];
	CDoubleLinkerInfo m_UnuseConns;

	// 更改GATE的连接验证机制
	// 之前为依据ip，现更改为id和ip共同验证
	int m_iCurrentUnRegisterNum;
	STUnRegisterSocketInfo m_astUnRegisterInfo[MAX_UNREGISTER_NUM];               // 最大未注册结构
	time_t m_tLastCheckTime;
};

#endif // _GATE_CTRL_HPP_
