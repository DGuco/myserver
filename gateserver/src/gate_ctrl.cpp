//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//


#include <share_mem.h>
#include "my_assert.h"
#include "../inc/gate_ctrl.h"

template<> std::shared_ptr<CGateCtrl> CSingleton<CGateCtrl>::spSingleton = NULL;

CGateCtrl::CGateCtrl()
	: m_pClientHandle(std::make_shared<CClientHandle>(m_pNetWork)),
	  m_pServerHandle(std::make_shared<CServerHandle>(m_pNetWork)),
	  m_pSingleThead(std::make_shared<CThreadPool>(1))
{
}

CGateCtrl::~CGateCtrl()
{
}

int CGateCtrl::PrepareToRun()
{
#ifdef _DEBUG_
	//初始化日志
	INIT_ROATING_LOG("default", "../log/gatesvrd.log", level_enum::trace);
#else
	//初始化日志
	INIT_ROATING_LOG("default", "../log/gatesvrd.log", level_enum::info);
#endif
	//读取配置文件
	ReadConfig();
	m_pClientHandle->PrepareToRun();
	m_pServerHandle->PrepareToRun();
	return 0;
}

int CGateCtrl::Run()
{
	LOG_INFO("default", "Libevent run with net module {}",
			 event_base_get_method(reinterpret_cast<const event_base *>(m_pNetWork->GetEventReactor()
				 ->GetEventBase())));
	//libevent事件循环
	m_pNetWork->DispatchEvents();
}

shared_ptr<CThreadPool> &CGateCtrl::GetSingleThreadPool()
{
	return m_pSingleThead;
}

shared_ptr<CClientHandle> &CGateCtrl::GetClientHandle()
{
	return m_pClientHandle;
}

shared_ptr<CServerHandle> &CGateCtrl::GetServerHandle()
{
	return m_pServerHandle;
}

shared_ptr<CNetWork> &CGateCtrl::GetNetWork()
{
	return m_pNetWork;
}

void CGateCtrl::ReadConfig()
{
	new CServerConfig;
	const string filePath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filePath)) {
		LOG_ERROR("default", "Get ServerConfig failed");
		exit(0);
	}
}

