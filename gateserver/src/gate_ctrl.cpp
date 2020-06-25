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
	: m_pNetManager(std::make_shared<CNetManager>()),
	  m_pMessManager(std::make_shared<CMessHandle>("CMessHandle", 1000 /*超时时间1ms*/))
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
	ReadConfig( );
	m_pMessManager->PrepareToRun( );
	m_pNetManager->PrepareToRun( );
	return 0;
}

int CGateCtrl::Run()
{
	//libevent事件循环
	m_pNetManager->DispatchEvents( );
}

shared_ptr<CNetManager> &CGateCtrl::GetNetManager()
{
	return m_pNetManager;
}

shared_ptr<CMessHandle> &CGateCtrl::GetMesManager()
{
	return m_pMessManager;
}

void CGateCtrl::ReadConfig()
{
    m_pConfig = CServerConfig::CreateInstance( );
	string filePath = "../config/serverinfo.json";
	if (-1 == m_pConfig->LoadFromFile(filePath)) {
		LOG_ERROR("default", "Get ServerConfig failed");
		exit(0);
	}
}