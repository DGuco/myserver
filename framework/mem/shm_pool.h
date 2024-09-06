/*****************************************************************
* FileName:shm_pool.h
* Summary :
* Date	  :2024-9-4
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __SHM_POOL_H__
#define __SHM_POOL_H__

#include "base.h"
#include "safe_pointer.h"

enum ShmObjStatus
{
	eShmObj_Free = 0,
	eShmObj_Used = 1,
	eShmObj_Changed = 2,
	eShmObj_Saved = 3,
	eShmObj_Deleted = 4,
};

template<typename T>
struct CShmObj
{
	int		m_nDbVersion;  //db 版本
	int		m_nPoolId;	   //对象池子索引位置
	bool    m_bIsValid; 
	T		m_Obj;
};

template<typename T>
class CShmPool
{
public:
	CShmPool();
	~CShmPool();
	//
	bool Init(int poolkey, int poolsize);
	//
	CSafePtr<CShmObj<T>> NewObj();
	//
	CSafePtr<CShmObj<T>> GetObj(int index);
	//
	int GetMaxSize();
private:
	CShmObj<T>**	m_pObjList;
	CSharedMem		m_ShareMem;
	int				m_nMaxSize;
	int				m_nUsedIndex;
};

template<typename T>
CShmPool::CShmPool()
{
	m_pObjList = NULL;
	m_nMaxSize = 0;
	m_nUsedIndex = 0;
}

template<typename T>
CShmPool::~CShmPool()
{

}

template<typename T>
bool CShmPool::Init(int poolkey, int poolsize)
{
	int tmMemSize = sizeof(CShmObj<T>) * poolsize;
	if (!m_ShareMem.CreateSegment(poolkey, tmMemSize))
	{
		if (!m_ShareMem.AttachSegment(poolkey, tmMemSize))
		{
			return  false;
		}
	}

	m_nMaxSize = poolsize;
	for (int index = 0; index < m_nMaxSize; index++)
	{
		m_pObjList[index] = m_ShareMem.GetSegment() + index * sizeof(CShmObj<T>);
	}
	return true;
}

template<typename T>
CSafePtr<CShmObj<T>> CShmPool::NewObj()
{
	if (m_nUsedIndex >= 0 && m_nUsedIndex < m_nMaxSize)
	{
		CSafePtr<CShmObj<T>> pObj = m_pObjList[m_nUsedIndex];
		m_nUsedIndex++;
		return pObj;
	}
	return NULL;
}


template<typename T>
CSafePtr<CShmObj<T>> CShmPool::GetObj(int index)
{
	if (index >= 0 && index < m_nMaxSize)
	{
		return m_pObjList[m_nUsedIndex];
	}
	return NULL;
}

#endif //__SHM_POOL_H__
