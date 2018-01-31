//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//


#include "../inc/gate_ctrl.h"

CGateCtrl::~CGateCtrl()
	: m_pNetWork(new CNetWork(eNetModule::NET_EPOLL)),
	  m_pC2sHandle(new CC2sHandle),
	  m_pS2cHandle(new CS2cHandle)
{
}

int CGateCtrl::Run()
{
	m_pC2sHandle->CreateThread();
	m_pS2cHandle->CreateThread();
	while (true) {
		m_pS2cHandle->CheckData();
		usleep(1000);
	}
}

int CGateCtrl::PrepareToRun()
{
	//初始化日志
	INIT_ROLLINGFILE_LOG("default", "../log/gatesvrd.log", LEVEL_DEBUG);
	CServerConfig *pTmpConfig = new CServerConfig;
	const string filepath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filepath)) {
		LOG_ERROR("default", "Get TcpserverConfig failed");
		exit(0);
	}
	return 0;
}
