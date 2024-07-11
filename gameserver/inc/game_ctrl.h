/*****************************************************************
* FileName:game_ctrl.h
* Summary :
* Date	  :2024-3-19
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __GAME_CTRL_H__
#define __GAME_CTRL_H__

#include "thread_scheduler.h"
#include "singleton.h"

class CGameCtrl : public CSingleton<CGameCtrl>
{
public:
	//构造函数
	CGameCtrl();
	//析构函数
	~CGameCtrl();
	//准备run
	bool PrepareToRun();
	//run
	int Run();
	//
	bool ReadConfig();
private:
	CSafePtr<CThreadScheduler> m_pScheduler;
};

#endif //__GAME_CTRL_H__
