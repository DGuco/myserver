#include "db_ctrl.h"
#include "common_def.h"
#include "thread_scheduler.h"

CDBCtrl::CDBCtrl()
{
    m_pScheduler = new CThreadScheduler("GameLogicScheduler");
	m_pSchedulerDb = new CThreadScheduler("DBLogicScheduler");
}

CDBCtrl::~CDBCtrl()
{

}

int CDBCtrl::Init()
{
    if(!m_pSchedulerDb->Init(8))
    {
        return -1;
    }
    return 0;
}

int CDBCtrl::PrepareToRun()
{
    return 0;
}