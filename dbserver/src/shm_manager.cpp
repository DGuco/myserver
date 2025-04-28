#include "shm_manager.h"
#include "db_playerdata.h"
#include "my_assert.h"
#include "shm_manager.h"

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

void CShmManager::DoSavePlayer(CSafePtr<IDataBase> pDataBase)
{
    if(m_ShmPool[enShmType_Player] != NULL)
    {
        m_ShmPool[enShmType_Player]->DoSave(pDataBase);
    }
}

void CShmManager::DoSaveGlobal(CSafePtr<IDataBase> pDataBase)
{
    for (size_t i = 0; i < enShmType_Max; i++)
    {
        if (m_ShmPool[i] != NULL && m_ShmPool[i]->GetShmType() != enShmType_Player)
        {
            m_ShmPool[i]->DoSave(pDataBase);
        }
        
    }
}

