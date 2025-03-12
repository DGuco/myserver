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
    CPlayerDataShmPool* pPlayerDataShmPool = new CPlayerDataShmPool();
    ASSERT_EX(pPlayerDataShmPool,"pPlayerDataShmPool is NULL");
    int bRet = pPlayerDataShmPool->Init(10000,enShmType_Player,50000,true);
    ASSERT_EX(bRet == 0,"pPlayerDataShmPool->Init() failed");
    RegisterShmPool(pPlayerDataShmPool);
    return 0;
}

void CShmManager::RegisterShmPool(IShmPool* pShmPool)
{
    ASSERT_EX(pShmPool,"pShmPool is NULL");
    int nShmType = pShmPool->GetShmType();
    ASSERT_EX(nShmType >= 0 && nShmType < enShmType_Max,"nShmType is invalid");
    m_ShmPool[nShmType] = pShmPool; 
}
