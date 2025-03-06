/*****************************************************************
* FileName:shm_manager.h
* Summary :
* Date	  :2024-9-5
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __SHM_MANAGER_H__
#define __SHM_MANAGER_H__

#include "shm_type.h"
#include "safe_pointer.h"

class IShmPool
{
public:
	virtual void      PrepareSave() = 0;
	virtual void      DoSaveAll() = 0;
    virtual enShmType GetShmType() = 0;
};

class CShmManager
{
private:
	CShmManager();
	~CShmManager();
    void Init();
    void HeartBeat();
    void RegisterShmPool(IShmPool *pShmPool, enShmType eShmType);
private:
    CSafePtr<IShmPool> m_pShmPool[enShmType_Max];
};

#endif //__SHM_MANAGER_H__
