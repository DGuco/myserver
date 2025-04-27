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
#include "db_thread.h"

class CDBCtrl : public CSingleton<CDBCtrl>
{
public:
	CDBCtrl();
	~CDBCtrl();
	int PrepareToRun();
	void Run();
private:
	//CSafePtr<CDBThreadPool<DBPlayerData>> m_pDbPlayerThreadPool;
};

#endif


