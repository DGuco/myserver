/*****************************************************************
* FileName:shm_manager.h
* Summary :
* Date	  :2025-03-12
* Author  :DGuco(1139140929@qq.com)
******************************************************************/

#ifndef SHM_MANAGER_H
#define SHM_MANAGER_H

#include "singleton.h"
#include "shm_pool.h"
#include "safe_pointer.h"
#include "shm_def.h"
#include "safe_pointer.h"
#include "thread_scheduler.h"

struct CShmPoolnfo
{
    enShmType                   shmType;
    CSafePtr<CThreadScheduler>  ownScheduler;
    CSafePtr<IShmPool>          shmPool;
    CShmPoolnfo()
    {
        shmType = enShmType_Invalid;
        ownScheduler.Reset();
        shmPool.Reset();
    }
};

class CShmManager : public CSingleton<CShmManager>
{
public:
    //
	CShmManager();
	//
    ~CShmManager();
    //
    int Init();
    //
    void RegisterShmPool(CSafePtr<IShmPool> pShmPool);
    //
    void DoSavePlayer(CSafePtr<IDataBase> pDataBase);
    //
    void DoSaveGlobal(CSafePtr<IDataBase> pDataBase);
private:
    CSafePtr<IShmPool> m_ShmPool[enShmType_Max];
};

#endif //SHM_MANAGER_H
