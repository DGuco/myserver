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

	return true;
}

bool CGameCtrl::Run()
{
	if (!m_pScheduler->Init(1, ThreadFuncParamWrapper(&CGameCtrl::InitTcp, NULL), ThreadFuncParamWrapper(&CGameCtrl::TcpTick, NULL)))
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

	auto task1 = m_pScheduler->Schedule("CombineTask1",
		[a, b]
		{
			return a + b;
		});

	auto task2 = m_pSchedulerDb->Schedule("CombineTask2",
		[]
		{
			return "Task2 execute done ";
		});

	auto task3 = m_pScheduler->Schedule("CombineTask3",
		[a, b]
		{
			return a / b;
		});

	auto task4 = m_pSchedulerDb->Schedule("CombineTask4",
		[a, b]
		{
			return a % b;
		});

	m_pSchedulerDb->AcceptCombine(task1, task2, task3, task4)
		.AcceptAll(
			[](int res1, std::string res2, int res3, int res4)
			{
				int nRes = res1 + res3 + res4;
				CACHE_LOG(DEBUG_CACHE, "Task res :{},combineres = {}", res2, nRes);
			});

// 	m_pSchedulerDb->ApplyCombine(task1, task2, task3, task4)
// 		.ApplyAll([]()
// 			{
// 				CACHE_LOG(DEBUG_CACHE, " ApplyAll ApplyAll ApplyAll ");
// 			});
	while (true)
	{
		SLEEP(10);
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

void CGameCtrl::TcpTick(void* args)
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

//
void CGameCtrl::InitTcp(void* args)
{
	if (!CGameServer::GetSingletonPtr()->InitTcp())
	{
		DISK_LOG(ERROR_DISK, "CGameServer::GetSingletonPtr()->InitTcp failed");
		exit(0);
	}
}