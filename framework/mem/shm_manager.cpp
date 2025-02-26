#include "shm_manager.h"
#include "shm_pool.h"
#include "shm_type.h"

CShmManager::CShmManager()
{

}

CShmManager::~CShmManager()
{

}

void CShmManager::Init()
{

}
void CShmManager::HeartBeat()
{

}

void CShmManager::RegisterShmPool(IShmPool *pShmPool, enShmType eShmType)
{
	m_pShmPool[eShmType] = pShmPool;
}
