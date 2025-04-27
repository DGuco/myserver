/*****************************************************************
* FileName:shm_pool.h
* Summary :
* Date	  :2024-9-4
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __SHM_POOL_H__
#define __SHM_POOL_H__

#include "base.h"
#include "safe_pointer.h"
#include "time_helper.h"
#include "shm_def.h"
#include "shm.h"
#include "database.h"
#include <atomic>

enum ShmObjStatus
{
	eShmObj_Free = 0,
	eShmObj_Used = 1,
	eShmObj_Changed = 2,
	eShmObj_Saveing = 4,
	eShmObj_Saved = 5,
	eShmObj_Deleted = 6,
};

template<typename T>
struct CShmObj
{
	int					m_nDbVersion;  //db 版本
	int					m_nPoolId;	   //对象池子索引位置
	bool   	 			m_bIsValid; 
	CACHE_LINE_ALIGN T	m_Obj;
	std::atomic_uchar	m_bStatus;

	CShmObj()
	{
		Clear();
	}

	void Clear()
	{
		m_nDbVersion = 0;
		m_nPoolId = -1;
		m_bIsValid = false;
		m_bStatus = eShmObj_Free;
	}
};

template<typename T>
struct CSavingObj
{
	CACHE_LINE_ALIGN T	m_Obj;
	std::atomic_uchar	m_bStatus;  //保存对象状态
	int					m_nSavingIndex;   //保存对象索引

	CSavingObj()
	{
		Clear();
	}

	void Clear()
	{
		m_nSavingIndex = -1;
		m_bStatus = eShmObj_Free;
	}
};

class IShmPool
{
public:
	virtual void      			PrepareSave() = 0;
    virtual enShmType 			GetShmType() = 0;
	virtual void      			DoSave(CSafePtr<IDataBase> pDataBase) = 0;
};
 
template<typename T,size_t poolsize,size_t cachesize>
class CShmPool : public IShmPool
{
public:
	CShmPool();
	virtual ~CShmPool();
	//
	bool Init(int poolkey,enShmType eShmType,int saveInterval,bool forceSaveAll);
	//
	CSafePtr<CShmObj<T>> NewObj();
	//
	CSafePtr<CShmObj<T>> GetObj(int index);
	//
	int GetMaxSize();
	//
	virtual void PrepareSave();
	//
	virtual void DoSave(CSafePtr<IDataBase> pDataBase);
private:
	CShmObj<T>*				m_pObjList[poolsize];
	CSavingObj<T>*			m_pSavingObj[cachesize];
	int 					m_nSaveInterval; //保存间隔时间
	CMyTimer				m_SaveTimer;  //保存定时器
	byte					m_bSaveAllFlag;	//保存所有对象的状态
	bool					m_bForceSaveAll;
	int						m_nSavingIndex;   //保存对象索引
	CSharedMem				m_ShareMem;
	int						m_nUsedIndex;
};

template<typename T,size_t poolsize,size_t cachesize>
CShmPool<T,poolsize,cachesize>::CShmPool()
{
	m_nUsedIndex = 0;
	m_nSavingIndex = 0;
	m_bForceSaveAll = 0;
	m_nSaveInterval = 0;
	m_bSaveAllFlag = 0;	

	for (size_t i = 0; i < poolsize; i++)
	{
		m_pObjList[i] = NULL;
	}

	for (size_t i = 0; i < cachesize; i++)
	{
		m_pSavingObj[i] = NULL;
	}
}

template<typename T,size_t poolsize,size_t cachesize>
CShmPool<T,poolsize,cachesize>::~CShmPool()
{

}

template<typename T,size_t poolsize,size_t cachesize>
bool CShmPool<T,poolsize,cachesize>::Init(int poolkey,enShmType eShmType,int saveInterval,bool forceSaveAll)
{
	m_bForceSaveAll = saveInterval;
	m_nSaveInterval = forceSaveAll;
	int tmMemSize = sizeof(CShmObj<T>) * poolsize + sizeof(CSavingObj<T>);
	if (!m_ShareMem.CreateSegment(poolkey, tmMemSize))
	{
		if (!m_ShareMem.AttachSegment(poolkey, tmMemSize))
		{
			return false;
		}
	}

	for (int index = 0; index < poolsize; index++)
	{
		m_pObjList[index] = (CShmObj<T>*)((m_ShareMem.GetSegment() + index * sizeof(CShmObj<T>)));
		m_pObjList[index]->Clear();
		m_pObjList[index]->m_bIsValid = true;
		m_pObjList[index]->m_nPoolId = index;
	}

	for (int index = 0; index < cachesize; index++)
	{
		m_pSavingObj[index] = (CSavingObj<T>*)((m_ShareMem.GetSegment() + sizeof(CShmObj<T>) * poolsize + index * sizeof(CSavingObj<T>)));
		m_pSavingObj[index]->Clear();
		m_pSavingObj[index]->m_bStatus = eShmObj_Free;
		m_pSavingObj[index]->m_nSavingIndex = -1;
	}

	time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	m_SaveTimer.BeginTimer(nNow, m_nSaveInterval);
	return true;
}

template<typename T,size_t poolsize,size_t cachesize>
CSafePtr<CShmObj<T>> CShmPool<T,poolsize,cachesize>::NewObj()
{
	if (m_nUsedIndex < 0 || m_nUsedIndex >= poolsize)	
	{
		return NULL;
	}
	CSafePtr<CShmObj<T>> pObj = m_pObjList[m_nUsedIndex];
	pObj->m_nPoolId = m_nUsedIndex;
	m_nUsedIndex++;
	return pObj;
}


template<typename T,size_t poolsize,size_t cachesize>
CSafePtr<CShmObj<T>> CShmPool<T,poolsize,cachesize>::GetObj(int index)
{
	if (index >= 0 && index < poolsize)
	{
		return m_pObjList[index];
	}
	return NULL;
}

template<typename T,size_t poolsize,size_t cachesize>
int CShmPool<T,poolsize,cachesize>::GetMaxSize()
{
	return poolsize;
}

template<typename T,size_t poolsize,size_t cachesize>
void CShmPool<T,poolsize,cachesize>::PrepareSave()
{
	time_t nNow  = CTimeHelper::GetSingletonPtr()->GetMSTime();
	if(m_SaveTimer.IsTimeout(nNow))
	{
		//开始保存所有发生改变的对象
		if(m_bSaveAllFlag == 0)
		{
			m_bSaveAllFlag = 1;
			m_nSavingIndex = 0;
			//m_SaveTimer.StopTimer();
		}
	}

	if(m_bSaveAllFlag == 0)
	{
		return;
	}

	for (size_t index = 0; index < cachesize; index++)
	{
		if(load_acquire(m_pSavingObj[index]->m_bStatus) == eShmObj_Saveing)
		{
			continue;
		}

		//找一个需要保存的对象
		size_t find_index = m_nSavingIndex;
		for (; find_index < poolsize; find_index++,m_nSavingIndex++)
		{
			if(load_acquire(m_pObjList[find_index]->m_bStatus) == eShmObj_Changed)
			{
				//拷贝对象
				m_pSavingObj[index]->m_Obj = m_pObjList[find_index]->m_Obj;
				store_release(m_pObjList[find_index]->m_bStatus,(unsigned char)eShmObj_Saved);
				//记录保存的内存池索引
				m_pSavingObj[index]->m_nSavingIndex = find_index;
				store_release(m_pSavingObj[index]->m_bStatus,(unsigned char)eShmObj_Saveing);
			}
		}
	}

	//保存完一遍了了，重置存储索引
	if(m_nSavingIndex >= poolsize)
	{
		m_nSavingIndex = 0;
		m_bSaveAllFlag = 0;
		//m_SaveTimer.ResetTimeout(nNow);
	}
}

template<typename T,size_t poolsize,size_t cachesize>
void CShmPool<T,poolsize,cachesize>::DoSave(CSafePtr<IDataBase> pDataBase)
{
	for (size_t index = 0; index < cachesize; index++)
	{
		if(load_acquire(m_pSavingObj[index]->m_bStatus) != eShmObj_Saveing)
		{
			continue;
		}

		try
        {
            bool nRet = m_pSavingObj[index]->m_Obj.Save(pDataBase);
            if(nRet)
            {
				store_release(m_pSavingObj[index]->m_bStatus,(unsigned char)eShmObj_Saved);
            }
            else
            {
				store_release(m_pSavingObj[index]->m_bStatus,(unsigned char)eShmObj_Free);
            }
        }	
        catch(const std::exception& e)
        {
			store_release(m_pSavingObj[index]->m_bStatus,(unsigned char)eShmObj_Free);
        }
	}
}
#endif //__SHM_POOL_H__