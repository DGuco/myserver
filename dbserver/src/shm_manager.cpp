#include "shm_manager.h"
#include "db_playerdata.h"
#include "my_assert.h"

CShmManager::CShmManager()
{
    for(int index = 0; index < enShmType_Max; ++index)
    {
        m_ShmPool[index] = NULL;
    }
}

CShmManager::~CShmManager()
{
}

int CShmManager::Init()
{
    CSafePtr<CPlayerDataShmPool> pPlayerDataShmPool = new CPlayerDataShmPool();
    ASSERT_EX(pPlayerDataShmPool != NULL,"pPlayerDataShmPool is NULL");
    int bRet = pPlayerDataShmPool->Init(10000,enShmType_Player,50000,true);
    ASSERT_EX(bRet == 0,"pPlayerDataShmPool->Init() failed");
    RegisterShmPool(pPlayerDataShmPool.DynamicCastTo<IShmPool>());
    return 0;
}

void CShmManager::RegisterShmPool(CSafePtr<IShmPool> pShmPool)
{
    ASSERT_EX(pShmPool != NULL,"pShmPool is NULL");
    int nShmType = pShmPool->GetShmType();
    ASSERT_EX(nShmType >= 0 && nShmType < enShmType_Max,"nShmType is invalid");
    m_ShmPool[nShmType] = pShmPool; 
}

void CShmManager::DoSave(enShmType shmType,CSafePtr<IDataBase> pDataBase)
{
    ASSERT_EX(shmType >= 0 && shmType < enShmType_Max,"shmType is invalid");
    if(m_ShmPool[shmType] != NULL)
    {
        m_ShmPool[shmType]->DoSave(pDataBase);
    }
}
