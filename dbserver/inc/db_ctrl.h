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

enum enDBThreadSchedulerType
{
    enDBScheduler_Invalid = -1,
	enDBScheduler_SaveHuman = 0,
    enDBScheduler_SaveGlobal = 1,
    enDBScheduler_Query = 2,
};

struct CBThreadSchedulerInfo
{	
	CSafePtr<CThreadScheduler> m_pScheduler;
	CSafePtr<DatabaseMysql>    m_pDatabase;
	enDBThreadSchedulerType    m_eSchedulerType;

	CBThreadSchedulerInfo(std::string signature,enDBThreadSchedulerType type)
	{
		m_eSchedulerType = type;
		m_pScheduler = new CThreadScheduler(signature);
		m_pDatabase = new DatabaseMysql();
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
	//db??
	CBThreadSchedulerInfo m_SaveHumanDbScheduler;
	//db??
	CBThreadSchedulerInfo m_SaveGlobalDbScheduler;
	//db??
	CBThreadSchedulerInfo m_QueryDbScheduler;
};

#endif


