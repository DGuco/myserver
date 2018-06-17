//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//


#include <my_macro.h>
#include <share_mem.h>
#include "my_assert.h"
#include "../inc/gate_ctrl.h"

template<> std::shared_ptr<CGateCtrl> CSingleton<CGateCtrl>::spSingleton = NULL;

CGateCtrl::CGateCtrl()
	: m_pC2sHandle(new CClientHandle()),
	  m_pS2cHandle(new CServerHandle),
	  m_pSingleThead(new CThreadPool(1))
{
}

CGateCtrl::~CGateCtrl()
{
	SAFE_DELETE(m_pC2sHandle);
	SAFE_DELETE(m_pS2cHandle);
	SAFE_DELETE(m_pSingleThead);
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
	m_pC2sHandle->PrepareToRun();
	m_pS2cHandle->PrepareToRun();
	return 0;
}

int CGateCtrl::Run()
{
	m_pS2cHandle->Run();
	m_pC2sHandle->Run();
}

shared_ptr<CThreadPool> CGateCtrl::GetSingleThreadPool()
{
	return m_pSingleThead;
}

shared_ptr<CClientHandle> CGateCtrl::GetCC2sHandle()
{
	return m_pC2sHandle;
}

shared_ptr<CServerHandle> CGateCtrl::GetCS2cHandle()
{
	return m_pS2cHandle;
}

void CGateCtrl::ReadConfig()
{
	new CServerConfig;
	const string filepath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filepath)) {
		LOG_ERROR("default", "Get TcpserverConfig failed");
		exit(0);
	}
}
