//
// dbctrl.h
// Created by DGuco on 17-7-13.
// Copyright ? 2018Äê DGuco. All rights reserved.
//

#ifndef _DBCTRL_HPP_
#define _DBCTRL_HPP_

#include "base.h"
#include "singleton.h"
#include "shm_manager.h"
#include "safe_pointer.h"
#include "my_thread.h"

class CDBCtrl : public CSingleton<CDBCtrl>
{
public:
	CDBCtrl();
	~CDBCtrl();
	//³õÊ¼»¯
	int Init();
private:
	CSafePtr<CThreadScheduler> m_pScheduler;
	CSafePtr<CThreadScheduler> m_pSchedulerDb;
};

#endif


