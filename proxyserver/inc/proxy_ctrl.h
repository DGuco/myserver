//
//  gatectrl.h
//  客户端tcp管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright ? 2016年 DGuco. All rights reserved.
//

#ifndef __PROXY_CTRL_H__
#define __PROXY_CTRL_H__

#include "base.h"
#include "proxy_def.h"
#include "tcp_server.h"
#include "safe_pointer.h"
#include "server_config.h"
#include "thread_scheduler.h"

#define MAX_TRANSFER_THREAD

struct CDBThreadInfo
{	
	int 						m_nThreadIndex;
	CDBThreadInfo()
	{
		m_nThreadIndex = -1;
	}
};

class CProxyCtrl: public CSingleton<CProxyCtrl>
{
public:
	//构造函数
	CProxyCtrl();
	//析构函数
	~CProxyCtrl();
	//准备run
	bool PrepareToRun();
	//run
	int Run();
private:
	//读取配置文件
	bool ReadConfig();
	//
	static void ProxyServerLogic(void* args);
	//
	static void ProxyServerInit(void* args);
	//
	static void TransferThreadLogic(void* args);
	//
	static void TransferThreadInit(void* args);
private:
	//tcp管理器
	CSafePtr<CThreadScheduler> m_pTcpManagerScheduler;
	//tcp管理器
	CSafePtr<CThreadScheduler> m_pTransferScheduler;
	//消息传输线程
	CSafePtr<CThreadScheduler> m_pMesTransferScheduler;
};

#endif
