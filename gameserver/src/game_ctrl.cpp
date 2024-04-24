#include "base.h"
#include "game_ctrl.h"
#include "game_server.h"
#include "server_config.h"
#include "log.h"

CGameCtrl::CGameCtrl()
{
#ifdef __WINDOWS__
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
#endif
}

CGameCtrl::~CGameCtrl()
{
#ifdef __WINDOWS__
	WSACleanup();
#endif
}

bool CGameCtrl::PrepareToRun()
{
	int a = 0;
	if (!ReadConfig())
	{
		return false;
	}

	if (!CGameServer::GetSingletonPtr()->PrepareToRun())
	{
		return false;
	}
	return true;
}

int CGameCtrl::Run()
{
	long long nTick = 0;
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetANSITime();
	while (true)
	{
		try
		{
			CGameServer::GetSingletonPtr()->TcpTick(nNow);
		}
		catch (const std::exception& e)
		{
			CACHE_LOG(ERROR_CACHE, "CGameServer TcpTick  cache execption msg {]", e.what());
		}

		nTick++;
		CACHE_LOG(DEBUG_CACHE, "CProxyServer::Run tick {}", nTick);
		SLEEP(1000);
	}
	return 0;
}

bool CGameCtrl::ReadConfig()
{
	string filePath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filePath))
	{
		DISK_LOG(ERROR_DISK, "Get ServerConfig failed");
		return false;
	}
	return true;
}