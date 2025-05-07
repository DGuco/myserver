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
#include "time_helper.h"

CProxyCtrl::CProxyCtrl()
{
#if defined(__WINDOWS__) || defined(_WIN32)
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#endif
}

CProxyCtrl::~CProxyCtrl()
{
#if defined(__WINDOWS__) || defined(_WIN32)
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

	//消息工厂注册
	CMessageFactoryManager::GetSingletonPtr()->Init();
	return true;
}

int CProxyCtrl::Run()
{
	if (!m_pTcpManagerScheduler->Init(1, &CProxyCtrl::ProxyServerInit,&CProxyCtrl::ProxyServerLogic,NULL,NULL))
	{
		return false;
	}

	long long nTick = 0;
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	while (true)
	{
		try
		{
			CProxyServer::GetSingletonPtr()->TcpTick(nNow);
		}
		catch (const std::exception& e)
		{
			CACHE_LOG(ERROR_CACHE,"CProxyServer TcpTick  cache execption msg {]", e.what());
		}

		CProxyServer::GetSingletonPtr()->CheckKickConn(nNow);
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

void CProxyCtrl::ProxyServerLogic(void* args)
{
    time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	try
	{
		CProxyServer::GetSingletonPtr()->TcpTick(nNow);
	}
	catch (const std::exception& e)
	{
		CACHE_LOG(ERROR_CACHE, "CProxyCtrl TcpTick catch execption msg {]", e.what());
	}
}

void CProxyCtrl::ProxyServerInit(void* args)
{
    if (!CProxyServer::GetSingletonPtr()->InitTcp())
	{
		DISK_LOG(ERROR_DISK, "CDBCtrl::GetSingletonPtr()->InitTcp failed");
		exit(0);
	}
}


void CProxyCtrl::TransferThreadLogic(void* args)
{
  
}

void CProxyCtrl::TransferThreadInit(void* args)
{

}