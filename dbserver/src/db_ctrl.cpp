#include "db_ctrl.h"
#include "db_server.h"
#include "common_def.h"
#include "shm_manager.h"
#include "server_config.h"
#include "thread_scheduler.h"

CDBCtrl::CDBCtrl() : 
    m_SaveHumanDbScheduler("SaveHumanDbScheduler",enDBScheduler_SaveHuman),
    m_SaveGlobalDbScheduler("SaveGlobalDbScheduler",enDBScheduler_SaveGlobal),
    m_QueryDbScheduler("QueryDbScheduler",enDBScheduler_Query)
{
    m_pTcpScheduler = new CThreadScheduler("DBTcpScheduler");
}

CDBCtrl::~CDBCtrl()
{

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
    return 0;
}

bool CDBCtrl::Run()
{
    if (!m_pTcpScheduler->Init(1, ThreadFuncParamWrapper(&CDBCtrl::InitTcp, NULL), ThreadFuncParamWrapper(&CDBCtrl::TcpTick, NULL)))
	{
		return false;
	}
    if (!m_SaveHumanDbScheduler.m_pScheduler->Init(1, 
        ThreadFuncParamWrapper(&CDBCtrl::DBThreadInit, (void*)(&m_SaveHumanDbScheduler)), 
        ThreadFuncParamWrapper(&CDBCtrl::DBThreadTick, (void*)(&m_SaveHumanDbScheduler))))
	{
		return false;
	}

    if (!m_SaveGlobalDbScheduler.m_pScheduler->Init(1, 
        ThreadFuncParamWrapper(&CDBCtrl::DBThreadInit, (void*)(&m_SaveGlobalDbScheduler)), 
        ThreadFuncParamWrapper(&CDBCtrl::DBThreadTick, (void*)(&m_SaveGlobalDbScheduler))))
	{
		return false;
	}

    if (!m_QueryDbScheduler.m_pScheduler->Init(4,   
        ThreadFuncParamWrapper(&CDBCtrl::DBThreadInit, NULL), 
        ThreadFuncParamWrapper(&CDBCtrl::DBThreadTick, NULL)))
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
    CBThreadSchedulerInfo* pInfo = (CBThreadSchedulerInfo*)args;
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
    CBThreadSchedulerInfo* pInfo = (CBThreadSchedulerInfo*)args;
    if(pInfo->m_pDatabase == NULL)
    {
        return;
    }

    if(pInfo->m_eSchedulerType == enDBScheduler_SaveHuman)
    {
        CShmManager::GetSingletonPtr()->DoSavePlayer(pInfo->m_pDatabase.DynamicCastTo<IDataBase>());
    }else if(pInfo->m_eSchedulerType == enDBScheduler_SaveGlobal)
    {
        CShmManager::GetSingletonPtr()->DoSaveGlobal(pInfo->m_pDatabase.DynamicCastTo<IDataBase>());
    }
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
