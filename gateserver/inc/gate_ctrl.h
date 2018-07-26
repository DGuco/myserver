//
//  gatectrl.h
//  客户端tcp管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef __GATE_CTRL_H__
#define __GATE_CTRL_H__

#include <sys/epoll.h>
#include "gate_def.h"
#include "log.h"
#include "server_tool.h"
#include "message.pb.h"
#include "base.h"
#include "config.h"
#include "net_work.h"
#include "client_manager.h"
#include "server_manager.h"
#include "thread_pool.h"

class CGateCtrl: public CSingleton<CGateCtrl>
{
public:
	//构造函数
	CGateCtrl();
	//析构函数
	~CGateCtrl();
	//准备run
	int PrepareToRun();
	//run
	int Run();
	//获取线程池
	shared_ptr<CThreadPool> &GetSingleThreadPool();
	shared_ptr<CClientManager> &GetClientManager();
	shared_ptr<CServerManager> &GetServerManager();
	shared_ptr<CNetWork> &GetNetWork();
private:
	//读取配置文件
	void ReadConfig();
private:
	shared_ptr<CNetWork> m_pNetWork;
	shared_ptr<CClientManager> m_pClientManager;
	shared_ptr<CServerManager> m_pServerManager;
	shared_ptr<CThreadPool> m_pSingleThead;
};

#endif
