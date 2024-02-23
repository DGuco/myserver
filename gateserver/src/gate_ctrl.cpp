//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include "my_assert.h"
#include "gate_ctrl.h"

CGateCtrl::CGateCtrl()
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
	return 0;
}

int CGateCtrl::Run()
{

}

void CGateCtrl::ReadConfig()
{
	string filePath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filePath)) 
	{
		LOG_ERROR("default", "Get ServerConfig failed");
		exit(0);
	}
}