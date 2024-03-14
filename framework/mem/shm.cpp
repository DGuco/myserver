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

CSharedMem::CSharedMem()
{
	m_pHead = NULL;
	m_pCurrentSegMent = NULL;
	m_InitMode = SHM_INVALID;
	m_nSize = 0;
}

CSharedMem::~CSharedMem()
{
	m_pHead = NULL;
	m_pCurrentSegMent = NULL;
	m_nSize = 0;
}

bool CSharedMem::Init(eShmModule module,sm_key nSmKey, size_t nSize)
{
	m_InitMode = module;
	if (m_InitMode == SHM_INIT)
	{
		return CreateSegment(nSmKey, nSize);
	}
	else if (m_InitMode == SHM_RESUME)
	{
		return AttachSegment(nSmKey, nSize);
	}
	return false;
}

eShmModule CSharedMem::GetInitMode()
{
	return m_InitMode;
}

bool CSharedMem::CreateSegment(sm_key nSmKey, size_t nSize)
{
	size_t smSize = nSize + sizeof(SSmHead);
	sm_handler handler = ShareMemAPI::CreateShareMem(nSmKey, smSize);
	if (handler == INVALID_SM_HADLER)
	{
		DISK_LOG(SHM_ERROR, "CreateSegment CreateShareMem failed nSmKey = {} error : {} errormsg : {}", nSmKey,errno,strerror(errno));
		return false;
	}
	BYTE* pAddr = ShareMemAPI::AttachShareMem(handler);
	if (pAddr == NULL)
	{
		DISK_LOG(SHM_ERROR, "AttachShareMem AttachShareMem failed nSmKey = {} error : {} errormsg : {}", nSmKey, errno,strerror(errno));
		return false;
	}
	m_pHead = ((SSmHead*)pAddr);
	m_pHead->m_nShmKey = nSmKey;
	m_pHead->m_nShmSize = nSize + sizeof(SSmHead);
	m_pHead->m_pSegment = pAddr;
	m_pHead->m_Handler = handler;
	m_pCurrentSegMent = ((BYTE*)(pAddr + sizeof(SSmHead)));
	m_nSize = nSize;
	DISK_LOG(SHM_DEBUG, "CSharedMem::CreateSegment OK nSmKey = {} size = {}", nSmKey, nSize);
	return true;
}

bool CSharedMem::AttachSegment(sm_key nSmKey, size_t nSize)
{
	size_t smSize = nSize + sizeof(SSmHead);
	sm_handler handler = ShareMemAPI::OpenShareMem(nSmKey, smSize);
	if (handler == INVALID_SM_HADLER)
	{
		DISK_LOG(SHM_ERROR, "AttachSegment OpenShareMem failed nSmKey = {} error : {} errormsg : {}", nSmKey, errno, strerror(errno));
		return false;
	}
	BYTE* pAddr = ShareMemAPI::AttachShareMem(handler);
	if (pAddr == NULL)
	{
		DISK_LOG(SHM_ERROR, "AttachSegment AttachShareMem failed nSmKey = {} error : {} errormsg : {}", nSmKey, errno, strerror(errno));
		return false;
	}
	m_pHead = ((SSmHead*)pAddr);
	m_pHead->m_nShmKey = nSmKey;
	m_pHead->m_nShmSize = nSize + sizeof(SSmHead);
	m_pHead->m_pSegment = pAddr;
	m_pHead->m_Handler = handler;
	m_pCurrentSegMent = ((BYTE*)(pAddr + sizeof(SSmHead)));
	m_nSize = nSize;
	DISK_LOG(SHM_DEBUG, "CSharedMem::AttachSegment OK nSmKey = {} size = {}", nSmKey, nSize);
	return true;
}

bool CSharedMem::DetachSegment()
{
	if (m_pHead != NULL)
	{
		if (!ShareMemAPI::DetachShareMem(m_pHead->m_pSegment))
		{
			DISK_LOG(SHM_ERROR, "DetachSegment DetachShareMem failed nSmKey = {} error : {} errormsg : {}", m_pHead->m_nShmKey, errno, strerror(errno));
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
			DISK_LOG(SHM_ERROR, "CloseSegment DetachShareMem failed nSmKey = {} error : {} errormsg : {}", m_pHead->m_nShmKey, errno, strerror(errno));
		}
	}
	if (m_pHead->m_Handler != NULL)
	{
		if (!ShareMemAPI::DestroyShareMem(m_pHead->m_Handler))
		{
			DISK_LOG(SHM_ERROR, "CloseSegment DestroyShareMem failed nSmKey = {} error : {} errormsg : {}", m_pHead->m_nShmKey, errno, strerror(errno));
		}
		else
		{
			ret = true;
		}
	}
	m_pHead = NULL;
	return ret;
}

BYTE* CSharedMem::GetSegment()
{
	return m_pCurrentSegMent;
}