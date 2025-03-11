#include "shm_manager.h"
#include "shm_pool.h"
#include "shm_def.h"
#include "my_assert.h"
#include "db_playerdata.h"

CShmManager::CShmManager()
{
}

CShmManager::~CShmManager()
{

}

void CShmManager::Init()
{
	CPlayerBaseShmPool* pPlayerShmPool = new CPlayerBaseShmPool();
	ASSERT_EX(pPlayerShmPool != NULL,"pPlayerShmPool == NULL");
	bool bRet = pPlayerShmPool->Init(10000,enShmType_Player,600,true);
	ASSERT_EX(bRet,"pPlayerShmPool->Init failed");
	RegisterShmPool(pPlayerShmPool, enShmType_Player);
}

void CShmManager::HeartBeat()
{

}

void CShmManager::RegisterShmPool(IShmPool *pShmPool, enShmType eShmType)
{
	m_pShmPool[eShmType] = pShmPool;
}