//
// dbctrl.h
// Created by DGuco on 17-7-13.
// Copyright ? 2018Äê DGuco. All rights reserved.
//

#ifndef _DBCTRL_HPP_
#define _DBCTRL_HPP_

#include "base.h"
#include "singleton.h"
#include "safe_pointer.h"
#include "db_playerdata.h"
#include "my_thread.h"
#include "database_mysql.h"
#include "thread_scheduler.h"

#define MAX_SAVEHUMAN_DB_THREAD 1
#define MAX_QUERY_DB_THREAD 4
#define MAX_SAVE_GLOBAL_DB_THREAD 1

enum enDBThreadType
{
    enDBThread_Invalid = -1,
	enDBThread_SaveHuman = 0,
    enDBThread_SaveGlobal = 1,
    enDBThread_Query = 2,
};

struct CDBThreadInfo
{	
	CSafePtr<DatabaseMysql>     m_pDatabase;
	enDBThreadType    			m_eSchedulerType;
	int 						m_nThreadIndex;

	CDBThreadInfo()
	{
		m_eSchedulerType = enDBThread_Invalid;
		m_pDatabase.Reset();
		m_nThreadIndex = -1;
	}
};

class CDBCtrl : public CSingleton<CDBCtrl>
{
public:
	CDBCtrl();
	~CDBCtrl();
	int PrepareToRun();
	//
	bool Run();
	//
	bool ReadConfig();
	//
	static void DBThreadInit(void* args);
	//
	static void DBThreadTick(void* args);
	//
	static void TcpTick(void* args);
	//
	static void InitTcp(void* args);
private:
	//????
	CSafePtr<CThreadScheduler> m_pTcpScheduler;
	//????
	CSafePtr<CThreadScheduler> m_pSaveHumanDbScheduler;
	//????
	CSafePtr<CThreadScheduler> m_pSaveGlobalDbScheduler;
	//????
	CSafePtr<CThreadScheduler> m_pQueryDbScheduler;
	//db??
	CDBThreadInfo* m_SaveHumanDbThread[MAX_SAVEHUMAN_DB_THREAD];
	//db??
	CDBThreadInfo* m_SaveGlobalDbThread[MAX_QUERY_DB_THREAD];
	//db??
	CDBThreadInfo* m_QueryDbThread[MAX_SAVE_GLOBAL_DB_THREAD];
};

#endif


