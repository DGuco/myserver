#include "base.h"
#include "game_ctrl.h"
#include "game_server.h"
#include "server_config.h"
#include "log.h"

CGameCtrl::CGameCtrl()
{
#if defined(__WINDOWS__) || defined(_WIN32)
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
#endif
	m_pScheduler = new CThreadScheduler("GameLogicScheduler");
	m_pSchedulerDb = new CThreadScheduler("DBLogicScheduler");
}

CGameCtrl::~CGameCtrl()
{
#if defined(__WINDOWS__) || defined(_WIN32)
	WSACleanup();
#endif
}

bool CGameCtrl::PrepareToRun()
{
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

bool CGameCtrl::Run()
{
	if (!m_pScheduler->Init(1, &CGameCtrl::GameServerInit,&CGameCtrl::GameServerTick, NULL, NULL))
	{
		return false;
	}

	if (!m_pSchedulerDb->Init(1))
	{
		return false;
	}
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

void CGameCtrl::GameServerTick(void* args)
{
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	try
	{
		CGameServer::GetSingletonPtr()->TcpTick(nNow);
	}
	catch (const std::exception& e)
	{
		CACHE_LOG(ERROR_CACHE, "CGameCtrl TcpTick  cache execption msg {]", e.what());
	}
}

//
void CGameCtrl::GameServerInit(void* args)
{
	if (!CGameServer::GetSingletonPtr()->InitTcp())
	{
		DISK_LOG(ERROR_DISK, "CGameCtrl::GetSingletonPtr()->InitTcp failed");
		exit(0);
	}
}