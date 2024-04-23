//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright ? 2016年 DGuco. All rights reserved.
//

#include "my_assert.h"
#include "proxy_ctrl.h"
#include "proxy_server.h"
#include "mfactory_manager.h"

CProxyCtrl::CProxyCtrl()
{
#ifdef __WINDOWS__
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#endif
}

CProxyCtrl::~CProxyCtrl()
{
#ifdef __WINDOWS__
	WSACleanup();
#endif
}

bool CProxyCtrl::PrepareToRun()
{
	int a = 0;
	//读取配置文件
	if (!ReadConfig())
	{
		return false;
	}

	if (!CProxyServer::GetSingletonPtr()->PrepareToRun())
	{
		return false;
	}

	//消息工厂注册
	CMessageFactoryManager::GetSingletonPtr()->Init();
	return true;
}

int CProxyCtrl::Run()
{
	long long nTick = 0;
	while (true)
	{
		try
		{
			CProxyServer::GetSingletonPtr()->TcpTick();
		}
		catch (const std::exception& e)
		{
			CACHE_LOG(ERROR_CACHE,"CProxyServer TcpTick  cache execption msg {]", e.what());
		}

		nTick++;
		CACHE_LOG(DEBUG_CACHE, "CProxyServer::Run tick {}", nTick);
		SLEEP(1000);
	}
}

bool CProxyCtrl::ReadConfig()
{
	string filePath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filePath)) 
	{
		DISK_LOG(ERROR_DISK, "Get ServerConfig failed");
		return false;
	}
	return true;
}