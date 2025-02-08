/*****************************************************************
* FileName:shm_pool.h
* Summary :
* Date	  :2024-9-4
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __SHM_POOL_H__
#define __SHM_POOL_H__

#include "shm.h"
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
CShmPool<T>::CShmPool()
{
	m_pObjList = NULL;
	m_nMaxSize = 0;
	m_nUsedIndex = 0;
}

template<typename T>
CShmPool<T>::~CShmPool()
{

}

template<typename T>
bool CShmPool<T>::Init(int poolkey, int poolsize)
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
CSafePtr<CShmObj<T>> CShmPool<T>::NewObj()
{
	if (m_nUsedIndex < 0 || m_nUsedIndex >= m_nMaxSize)	
	{
		return NULL;
	}
	CSafePtr<CShmObj<T>> pObj = m_pObjList[m_nUsedIndex];
	pObj->m_nPoolId = m_nUsedIndex;
	m_nUsedIndex++;
	return pObj;
}


template<typename T>
CSafePtr<CShmObj<T>> CShmPool<T>::GetObj(int index)
{
	if (index >= 0 && index < m_nMaxSize)
	{
		return m_pObjList[index];
	}
	return NULL;
}

template<typename T>
int CShmPool<T>::GetMaxSize()
{
	return m_nMaxSize;
}


#endif //__SHM_POOL_H__
