#include "db_ctrl.h"
#include "common_def.h"
#include "thread_scheduler.h"

CDBCtrl::CDBCtrl()
{
    m_pDbPlayerThreadPool = new CDBThreadPool<DBPlayerData>();
}

CDBCtrl::~CDBCtrl()
{

}

int CDBCtrl::PrepareToRun()
{
    m_pDbPlayerThreadPool->Init(1);
    return 0;
}