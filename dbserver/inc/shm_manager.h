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

class CShmManager : public CSingleton<CShmManager>
{
public:
	CShmManager();
	~CShmManager();
    int Init();
    void RegisterShmPool(CSafePtr<IShmPool> pShmPool);
private:
    CSafePtr<IShmPool> m_ShmPool[enShmType_Max];
};

#endif //SHM_MANAGER_H
