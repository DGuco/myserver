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
	m_pSchedulerDb = new CThreadScheduler("DBLogicScheduler");
}

CGameCtrl::~CGameCtrl()
{
#ifdef __WINDOWS__
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

	if (!m_pScheduler->Init(1))
	{
		return false;
	}

	if (!m_pSchedulerDb->Init(1))
	{
		return false;
	}
	int a = 1;
	int b = 2;
	int c = 10;
	int d = 20;
	m_pScheduler->Schedule("InitTcpServer",
		[a, b]
		{
			if (!CGameServer::GetSingletonPtr()->InitTcp())
			{
				exit(0);
			}
			return a + b;
		}
		)
	    .ThenAccept(m_pSchedulerDb,
			[c, d] (int res)
			{
				int finres = c + d + res;
				printf("finres = %d\n", finres);
				throw std::runtime_error("run time error");
			})
		.ThenApply(m_pScheduler,
			[c, d]
			{
				int finres = c + d;
				printf("finres = %d\n", finres);
			});
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
		SLEEP(1000);
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