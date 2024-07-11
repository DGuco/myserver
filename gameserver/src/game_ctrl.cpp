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
	m_pScheduler = new CThreadScheduler("GameLogicScheduler");
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

	if (!m_pScheduler->Init(1))
	{
		return false;
	}
	if (!CGameServer::GetSingletonPtr()->PrepareToRun())
	{
		return false;
	}

	m_pScheduler->Schedule("InitTcpServer",
		[]
		{
			if (!CGameServer::GetSingletonPtr()->InitTcp())
			{
				exit(0);
			}
		}
		);
	return true;
}

int CGameCtrl::Run()
{
	while (true)
	{
		m_pScheduler->Schedule("GameLogic",
			[]
			{
				time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
				try
				{
					CGameServer::GetSingletonPtr()->TcpTick(nNow);
				}
				catch (const std::exception& e)
				{
					CACHE_LOG(ERROR_CACHE, "CGameServer TcpTick  cache execption msg {]", e.what());
				}
			}
			);
		m_pScheduler->DebugTask();
		SLEEP(100);
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