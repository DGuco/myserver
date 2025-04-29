#include "db_ctrl.h"
#include "db_server.h"
#include "common_def.h"
#include "shm_manager.h"
#include "server_config.h"
#include "thread_scheduler.h"
#include "base.h"

CDBCtrl::CDBCtrl() :
     m_pTcpScheduler(NULL),
     m_pSaveHumanDbScheduler(NULL), 
     m_pSaveGlobalDbScheduler(NULL), 
     m_pQueryDbScheduler(NULL)
{
    m_pTcpScheduler = new CThreadScheduler("DBTcpScheduler");
    m_pSaveHumanDbScheduler = new CThreadScheduler("SaveHumanDbScheduler");
    m_pSaveGlobalDbScheduler = new CThreadScheduler("SaveGlobalDbScheduler");
    m_pQueryDbScheduler = new CThreadScheduler("QueryDbScheduler");
   
    for (int i = 0; i < MAX_SAVEHUMAN_DB_THREAD; i++)
    {
        m_SaveHumanDbThread[i] = new CDBThreadInfo();
        m_SaveHumanDbThread[i]->m_eSchedulerType = enDBThread_SaveHuman;
        m_SaveHumanDbThread[i]->m_pDatabase = new DatabaseMysql();
        m_SaveHumanDbThread[i]->m_nThreadIndex = i;
    }

    for (int i = 0; i < MAX_SAVE_GLOBAL_DB_THREAD; i++)
    {
        m_SaveGlobalDbThread[i] = new CDBThreadInfo();
        m_SaveGlobalDbThread[i]->m_eSchedulerType = enDBThread_SaveGlobal;
        m_SaveGlobalDbThread[i]->m_pDatabase = new DatabaseMysql();
        m_SaveGlobalDbThread[i]->m_nThreadIndex = i;
    }

    for (int i = 0; i < MAX_QUERY_DB_THREAD; i++)
    {
        m_QueryDbThread[i] = new CDBThreadInfo();
        m_QueryDbThread[i]->m_eSchedulerType = enDBThread_Query;
        m_QueryDbThread[i]->m_pDatabase = new DatabaseMysql();
        m_QueryDbThread[i]->m_nThreadIndex = i;
    }
}

CDBCtrl::~CDBCtrl()
{
    m_pTcpScheduler.Free();
    m_pSaveHumanDbScheduler.Free();
    m_pSaveGlobalDbScheduler.Free();
    m_pQueryDbScheduler.Free();
    for (int i = 0; i < MAX_SAVEHUMAN_DB_THREAD; i++)
    {
        m_SaveHumanDbThread[i]->m_pDatabase.Free();
        delete m_SaveHumanDbThread[i];
    }

    for (int i = 0; i < MAX_SAVE_GLOBAL_DB_THREAD; i++)
    {
        m_SaveGlobalDbThread[i]->m_pDatabase.Free();
        delete m_SaveGlobalDbThread[i];
    }

    for (int i = 0; i < MAX_QUERY_DB_THREAD; i++)
    {
        m_QueryDbThread[i]->m_pDatabase.Free();
        delete m_QueryDbThread[i];
    }
}

int CDBCtrl::PrepareToRun()
{
    if (!ReadConfig())
	{
		return false;
	}

	if (!CDBSerer::GetSingletonPtr()->PrepareToRun())
	{
		return false;
	}

	return true;
}

bool CDBCtrl::Run()
{
    if (!m_pTcpScheduler->Init(1, &CDBCtrl::InitTcp,&CDBCtrl::TcpTick,NULL,NULL))
	{
		return false;
	}
    if (!m_pSaveHumanDbScheduler->Init(MAX_SAVEHUMAN_DB_THREAD, 
                                        &CDBCtrl::DBThreadInit, 
                                        &CDBCtrl::DBThreadTick, 
                                        (void**)&m_SaveHumanDbThread,
                                        (void**)&m_SaveHumanDbThread))
	{
		return false;
	}

    if (!m_pSaveGlobalDbScheduler->Init(MAX_SAVE_GLOBAL_DB_THREAD, 
                                        &CDBCtrl::DBThreadInit,
                                        &CDBCtrl::DBThreadTick,
                                        (void**)&m_SaveGlobalDbThread, 
                                        (void**)&m_SaveGlobalDbThread))
	{
		return false;
	}

    if (!m_pQueryDbScheduler->Init(MAX_QUERY_DB_THREAD, 
                                    &CDBCtrl::DBThreadInit,
                                    &CDBCtrl::DBThreadTick,
                                    (void**)&m_QueryDbThread, 
                                    (void**)&m_QueryDbThread))
	{
		return false;
	}

    while (1)
    {
        SLEEP(10);
    }
}

void CDBCtrl::DBThreadInit(void* args)
{
    if(args == NULL)
    {
		DISK_LOG(ERROR_DISK, "CDBCtrl::DBThreadInit failed, args == NULL");
        exit(0);
    }
    CDBThreadInfo* pInfo = (CDBThreadInfo*)args;
    if(pInfo->m_pDatabase == NULL)
    {
        DISK_LOG(ERROR_DISK, "CDBCtrl::DBThreadInit failed, m_pDatabase == NULL");
        exit(0);
    }
    
    if(!pInfo->m_pDatabase->Initialize("127.0.0.1;3306;root;000000;test",2))
    {
        DISK_LOG(ERROR_DISK, "CDBCtrl::DBThreadInit failed, m_pDatabase->Init() failed");
        exit(0);
    }
}

void CDBCtrl::DBThreadTick(void* args)
{
    if(args == NULL)
    {
		return;
    }
    CDBThreadInfo* pInfo = (CDBThreadInfo*)args;
    if(pInfo->m_pDatabase == NULL)
    {
        return;
    }

    if(pInfo->m_eSchedulerType == enDBThread_SaveHuman)
    {
        CShmManager::GetSingletonPtr()->DoSavePlayer(pInfo->m_pDatabase.DynamicCastTo<IDataBase>());
    }else if(pInfo->m_eSchedulerType == enDBThread_SaveGlobal)
    {
        CShmManager::GetSingletonPtr()->DoSaveGlobal(pInfo->m_pDatabase.DynamicCastTo<IDataBase>());
    }else
    {
        return;
    }
    return;
}

void CDBCtrl::TcpTick(void* args)
{
    time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	try
	{
		CDBSerer::GetSingletonPtr()->TcpTick(nNow);
	}
	catch (const std::exception& e)
	{
		CACHE_LOG(ERROR_CACHE, "CDBCtrl TcpTick  cache execption msg {]", e.what());
	}
}

void CDBCtrl::InitTcp(void* args)
{
    if (!CDBSerer::GetSingletonPtr()->InitTcp())
	{
		DISK_LOG(ERROR_DISK, "CDBCtrl::GetSingletonPtr()->InitTcp failed");
		exit(0);
	}
}

bool CDBCtrl::ReadConfig()
{
	string filePath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filePath))
	{
		DISK_LOG(ERROR_DISK, "Get ServerConfig failed");
		return false;
	}
	return true;
}
