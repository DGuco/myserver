#include "shm_api.h"
#include "shm.h"
#include "log.h"

using namespace ShareMemAPI;
// /**
//   *函数名          : CSharedMem
//   *功能描述        : 改变CSharedMem的new操作，在共享内存空间上生成对象用来进程间通信，
//   *   				pbCurrentShm指向共享内存块地址，大小通常为sizeof(CSharedMem)
//   *   				+ sizeof(CCodeQueue) + PIPE_SIZE（可变）
//   *参数           ： sizeof（CSharedMem）
// **/
// void *CSharedMem::operator new(size_t nSize)
// {
// 	BYTE *pTemp = NULL;
// 
// 	if (!pbCurrentShm) {
// 		return (void *) NULL;
// 	}
// 
// 	pTemp = pbCurrentShm;
// 
// 	return (void *) pTemp;
// }
// 
// void CSharedMem::operator delete(void *pMem)
// {
// }
// 
// CSharedMem::CSharedMem()
// {
// 	m_pbCurrentSegMent = pbCurrentShm + sizeof(CSharedMem);
// }

CSharedMem::CSharedMem(EIMode module)
{
	m_pHead = NULL;
	m_pCurrentSegMent = NULL;
	m_InitMode = module;
	m_nSize = 0;
	m_Handler = NULL;
}

CSharedMem::~CSharedMem()
{
	m_pHead = NULL;
	m_pCurrentSegMent = NULL;
	m_nSize = 0;
}

bool CSharedMem::Init(sm_key nSmKey, size_t nSize)
{
	if (m_InitMode == SHM_INIT)
	{
		return CreateSegment(nSmKey, nSize);
	}
	else if (m_InitMode == SHM_RECOVER)
	{
		return AttachSegment(nSmKey, nSize);
	}
	return false;
}

EIMode CSharedMem::GetInitMode()
{
	return m_InitMode;
}

bool CSharedMem::CreateSegment(sm_key nSmKey, size_t nSize)
{
	size_t smSize = nSize + sizeof(SSmHead);
	m_Handler = ShareMemAPI::CreateShareMem(nSmKey, smSize);
	if (m_Handler == NULL)
	{
		LOG_ERROR("default", "CreateSegment CreateShareMem failed nSmKey = {} error = {}", nSmKey, errno);
		return false;
	}
	BYTE* pAddr = ShareMemAPI::AttachShareMem(m_Handler);
	if (pAddr == NULL)
	{
		LOG_ERROR("default", "AttachShareMem AttachShareMem failed nSmKey = {} error = {}", nSmKey, errno);
		return false;
	}
	m_pHead.Reset((SSmHead*)pAddr);
	m_pHead->m_nShmKey = nSmKey;
	m_pHead->m_nShmSize = nSize + sizeof(SSmHead);
	m_pHead->m_pSegment = pAddr;
	m_pCurrentSegMent.Reset((BYTE*)(pAddr + sizeof(SSmHead)));
	m_nSize = nSize;
	LogDebug("default", "CSharedMem::CreateSegment OK nSmKey = {} size = {}", nSmKey, nSize);
	return true;
}

bool CSharedMem::AttachSegment(sm_key nSmKey, size_t nSize)
{
	size_t smSize = nSize + sizeof(SSmHead);
	m_Handler = ShareMemAPI::OpenShareMem(nSmKey, smSize);
	if (m_Handler == (sm_handler)(-1))
	{
		LOG_ERROR("default", "AttachSegment OpenShareMem failed nSmKey = {} error = {}", nSmKey, errno);
		return false;
	}
	BYTE* pAddr = ShareMemAPI::AttachShareMem(m_Handler);
	if (pAddr == NULL)
	{
		LOG_ERROR("default", "AttachSegment AttachShareMem failed nSmKey = {} error = {}", nSmKey, errno);
		return false;
	}
	m_pHead.Reset((SSmHead*)pAddr);
	m_pHead->m_nShmKey = nSmKey;
	m_pHead->m_nShmSize = nSize + sizeof(SSmHead);
	m_pHead->m_pSegment = pAddr;
	m_pCurrentSegMent.Reset((BYTE*)(pAddr + sizeof(SSmHead)));
	m_nSize = nSize;
	LogDebug("default", "CSharedMem::AttachSegment OK nSmKey = {} size = {}", nSmKey, nSize);
	return true;
}

bool CSharedMem::DetachSegment()
{
	if (m_pHead != NULL)
	{
		if (!ShareMemAPI::DetachShareMem(m_pHead->m_pSegment))
		{
			LOG_ERROR("default", "DetachSegment DetachShareMem failed nSmKey = {} error = {}", m_pHead->m_nShmKey, errno);
			return false;
		}
		return true;
	}
	return false;
}

bool CSharedMem::CloseSegment()
{
	bool ret = 0;
	if (m_pHead != NULL)
	{
		//先detach 内存映射
		if (!ShareMemAPI::DetachShareMem(m_pHead->m_pSegment))
		{
			LOG_ERROR("default", "CloseSegment DetachShareMem failed nSmKey = {} error = {}", m_pHead->m_nShmKey, errno);
		}
	}
	if (m_Handler != NULL)
	{
		if (!ShareMemAPI::DestroyShareMem(m_Handler))
		{
			LOG_ERROR("default", "CloseSegment DestroyShareMem failed nSmKey = {} error = {}", m_pHead->m_nShmKey, errno);
		}
		else
		{
			ret = true;
		}
	}
	m_pHead = NULL;
	m_Handler = NULL;
	return ret;
}