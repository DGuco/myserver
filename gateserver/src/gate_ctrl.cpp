//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include "my_assert.h"
#include "gate_ctrl.h"
#include "gate_server.h"

CGateCtrl::CGateCtrl()
{
#ifdef __WINDOWS__
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
#endif
}

CGateCtrl::~CGateCtrl()
{
#ifdef __WINDOWS__
	WSACleanup();
#endif
}

bool CGateCtrl::PrepareToRun()
{
	if (!INIT_LOG("gateserver"))
	{
		return false;
	}
	int a = 0;
	//读取配置文件
	if (!ReadConfig())
	{
		return false;
	}

	if (!CGateServer::GetSingletonPtr()->PrepareToRun())
	{
		return false;
	}
	return true;
}

int CGateCtrl::Run()
{
	long long nTick = 0;
	while (true)
	{
		try
		{
			CGateServer::GetSingletonPtr()->TcpTick();
		}
		catch (const std::exception& e)
		{
			CACHE_LOG(ERROR_CACHE,"CGateServer TcpTick  cache execption msg {]", e.what());
		}

		try
		{
			CGateServer::GetSingletonPtr()->RecvGameData();
		}
		catch (const std::exception& e)
		{
			CACHE_LOG(ERROR_CACHE, "CGateServer RecvGameData  cache execption msg {]", e.what());
		}

		nTick++;
		CACHE_LOG(DEBUG_CACHE, "CGateCtrl::Run tick {}", nTick);
		SLEEP(10);
	}
}

bool CGateCtrl::ReadConfig()
{
	string filePath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filePath)) 
	{
		DISK_LOG(ERROR_DISK, "Get ServerConfig failed");
		return false;
	}
	return true;
}