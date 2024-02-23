//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include "my_assert.h"
#include "gate_ctrl.h"
#include "gate_server.h"
#include "mes_handle.h"

CGateCtrl::CGateCtrl()
{

}

CGateCtrl::~CGateCtrl()
{
}

bool CGateCtrl::PrepareToRun()
{
#ifdef _DEBUG_
	//初始化日志
	INIT_ROATING_LOG("default", "../log/gatesvrd.log", level_enum::trace);
#else
	//初始化日志
	INIT_ROATING_LOG("default", "../log/gatesvrd.log", level_enum::info);
#endif
	//读取配置文件
	if (!ReadConfig()) exit(0);
	if (!CMessHandle::GetSingletonPtr()->PrepareToRun()) exit(0);
	if (!CGateServer::GetSingletonPtr()->PrepareToRun()) exit(0);
	return true;
}

int CGateCtrl::Run()
{
	while (true)
	{
		try
		{
			CGateServer::GetSingletonPtr()->Run();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("default""CGateServer Run  cache execption msg {]", e.what());
		}

		try
		{
			CMessHandle::GetSingletonPtr()->Run();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("default""CMessHandle Run  cache execption msg {]", e.what());
		}
		SLEEP(10);
	}
}

bool CGateCtrl::ReadConfig()
{
	string filePath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filePath)) 
	{
		LOG_ERROR("default", "Get ServerConfig failed");
		return false;
	}
	return true;
}