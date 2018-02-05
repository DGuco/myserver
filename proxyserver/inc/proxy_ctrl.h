//
//  gate_ctrl.h
//  gate_ctrl 管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _PROXY_CTRL_HPP_
#define _PROXY_CTRL_HPP_

#include <mutex>
#include "proxy_handle.h"
#include "server_tool.h"

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
	CProxyHandle*		mpHandle;		// handle
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


class CProxyCtrl {
public:
	CProxyCtrl();
	~CProxyCtrl();

	enum _enParas
	{
		MUTEX_HASHMAP = 0,

		MUTEX_NUM = 1
	};

#ifdef _POSIX_MT_
	static std::mutex stLinkMutex[EHandleType_NUM]; 	//连接链表保护锁 注：在主线程和子线程之间起作用
	static std::mutex stMutex[MUTEX_NUM];				//连接hashmap保护锁 注：在主线程个子线程之间，以及各个子线程之间起作用
#endif

	// 初始化
	int Initialize();
	// 准备执行线程
	int PrepareToRun();
	// 执行线程
	int Run();

private:
	// 检测运行标志
	int CheckRunFlags();
	// 检测连接请求
	int CheckConnRequest();
	// 定时检查
	int CheckRoutines();	
	// 获取指定连接信息	
	CMyTCPConn* GetConnByAddrAndID(short shSrcID, unsigned long ulSrcAddr);
	// 接收并处理注册消息
	int ReceiveAndProcessRegister(int iUnRegisterIdx);
	// 删除一个未注册的连接	
	int DeleteOneUnRegister(int iUnRegisterIdx);		
	
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

	//gate监听socket信息
	CMyTCPSocket m_stListenSocket;
	//tcp连接数组
	CMyTCPConn m_astConns[MAX_CONNS_NUM];
	//当前的可用的连接map(注:线程之间共享数据)
	CONNS_MAP m_mapConns;

	//gate线程管理类数组
	CHandleInfo m_stHandleInfos[EHandleType_NUM];
	//空闲连接双向链表
	CDoubleLinkerInfo m_UnuseConns;

	// 当前注册数
	int m_iCurrentUnRegisterNum;
	// 最大未注册结构
	STUnRegisterSocketInfo m_astUnRegisterInfo[MAX_UNREGISTER_NUM];               
	time_t m_tLastCheckTime;
};

#endif // _PROXY_CTRL_HPP_
