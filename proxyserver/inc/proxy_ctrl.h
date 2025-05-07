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
#include "proxy_transfer.h"
#include "thread_scheduler.h"

#define MAX_TRANSFER_THREAD 4

struct CTransferThreadInfo
{	
	int 						m_nThreadIndex;
	CSafePtr<CProxyTransfer>    m_pTransfer;
	CTransferThreadInfo()
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
	//选择一个线程管理transfer
	void SelectTransferThread(CSocket socket);
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
	//
	CTransferThreadInfo* m_TransferThread[MAX_TRANSFER_THREAD];
};

#endif
