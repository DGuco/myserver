/*****************************************************************
* FileName:shm_manager.h
* Summary :
* Date	  :2024-9-5
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __SHM_MANAGER_H__
#define __SHM_MANAGER_H__

#include "shm_type.h"
#include "shm_pool.h"

class CShmManager
{
private:
	CShmManager();
	~CShmManager();
    void Init();
    void HeartBeat();
private:
    CSafePtr<IShmPool> m_pShmPool[enShmType_Max];
};

#endif //__SHM_MANAGER_H__
