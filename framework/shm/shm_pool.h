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
#include "../../framework/shm/shm.h"
#include <atomic>

enum ShmObjStatus
{
	eShmObj_Free = 0,
	eShmObj_Used = 1,
	eShmObj_Changed = 2,
	eShmObj_Saved = 3,
	eShmObj_Deleted = 4,
};

enum SavingStauts
{
	eSaveStatus_Free = 0,   //空闲
	eSaveStatus_Saving = 1, //保存中
	eSaveStatus_Saved = 2,	//保存完成
};

template<typename T>
struct CShmObj
{
	int					m_nDbVersion;  //db 版本
	int					m_nPoolId;	   //对象池子索引位置
	bool   	 			m_bIsValid; 
	CACHE_LINE_ALIGN T	m_Obj;
	std::atomic_uchar	m_bStatus;
	//对原子变量y进行acquire的load操作，因此变量y之后的store/load操作不能排序到y之前
	ShmObjStatus GetObjStatus()						{ return (ShmObjStatus)m_bStatus.load(std::memory_order_acquire); }
	//对原子变量y进行了release的store操作，因此y变量之前的store/load操作不能排序到y之后
	void SetObjStatus(ShmObjStatus state)			{ m_bStatus.store(m_bStatus, std::memory_order_release);}

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
	std::atomic_uchar	m_bSavingStatus;  //保存对象状态
	int					m_nSavingIndex;   //保存对象索引
	//对原子变量y进行acquire的load操作，因此变量y之后的store/load操作不能排序到y之前
	SavingStauts GetSavingStatus()						{ return m_bSavingStatus.load(std::memory_order_acquire); }
	//对原子变量y进行了release的store操作，因此y变量之前的store/load操作不能排序到y之后
	void SetSavingStatus(SavingStauts state)			{ m_bSavingStatus.store(m_bSavingStatus, std::memory_order_release);}

	CSavingObj()
	{
		Clear();
	}

	void Clear()
	{
		m_nSavingIndex = -1;
		m_bSavingStatus = eSaveStatus_Free;
	}
};

class IShmPool
{
public:
	virtual void      			PrepareSave() = 0;
    virtual enShmType 			GetShmType() = 0;
	virtual void      			DoSave() = 0;
};
 
template<typename T,size_t poolsize,size_t saving_size = 1>
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
	virtual void DoSave();
private:
	CShmObj<T>*				m_pObjList[poolsize];
	CSavingObj<T>*			m_SavingObjList[saving_size];
	int 					m_nSaveInterval; //保存间隔时间
	CMyTimer				m_SaveTimer;  //保存定时器
	byte					m_bSaveAllFlag;	//保存所有对象的状态
	bool					m_bForceSaveAll;
	int						m_nSavingIndex;   //保存对象索引
	CSharedMem				m_ShareMem;
	int						m_nUsedIndex;
};

template<typename T,size_t poolsize,size_t saving_size>
CShmPool<T,poolsize,saving_size>::CShmPool()
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
	
	for (size_t i = 0; i < saving_size; i++)
	{
		m_SavingObjList[i] = NULL;
	}
}

template<typename T,size_t poolsize,size_t saving_size>
CShmPool<T,poolsize,saving_size>::~CShmPool()
{

}

template<typename T,size_t poolsize,size_t saving_size>
bool CShmPool<T,poolsize,saving_size>::Init(int poolkey,enShmType eShmType,int saveInterval,bool forceSaveAll)
{
	m_bForceSaveAll = saveInterval;
	m_nSaveInterval = forceSaveAll;
	int tmMemSize = sizeof(CShmObj<T>) * poolsize + sizeof(CSavingObj<T>) * saving_size;
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

	for (int index = 0; index < saving_size; index++)
	{
		m_SavingObjList[index] = (CSavingObj<T>*)(m_ShareMem.GetSegment() + sizeof(CShmObj<T>) * poolsize + index * sizeof(CSavingObj<T>));
		m_SavingObjList[index]->Clear();
		m_SavingObjList[index]->m_bSavingStatus = eSaveStatus_Free;
		m_SavingObjList[index]->m_nSavingIndex = -1;
	}
	time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	m_SaveTimer.BeginTimer(nNow, m_nSaveInterval);
	return true;
}

template<typename T,size_t poolsize,size_t saving_size>
CSafePtr<CShmObj<T>> CShmPool<T,poolsize,saving_size>::NewObj()
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


template<typename T,size_t poolsize,size_t saving_size>
CSafePtr<CShmObj<T>> CShmPool<T,poolsize,saving_size>::GetObj(int index)
{
	if (index >= 0 && index < poolsize)
	{
		return m_pObjList[index];
	}
	return NULL;
}

template<typename T,size_t poolsize,size_t saving_size>
int CShmPool<T,poolsize,saving_size>::GetMaxSize()
{
	return poolsize;
}

template<typename T,size_t poolsize,size_t saving_size>
void CShmPool<T,poolsize,saving_size>::PrepareSave()
{
	time_t nNow  = CTimeHelper::GetSingletonPtr()->GetMSTime();
	if(m_SaveTimer.IsTimeout(nNow))
	{
		//开始保存所有发生改变的对象
		if(m_bSaveAllFlag == 0)
		{
			m_bSaveAllFlag = 1;
			m_nSavingIndex = 0;
			m_SaveTimer.StopTimer();
		}
	}

	if(m_bSaveAllFlag == 0)
	{
		return;
	}

	for(int index = 0;index < saving_size;index++)
	{
		CSavingObj<T>* pSavingObj = m_SavingObjList[index];
		size_t find_index = m_nSavingIndex;
		if (pSavingObj->m_bSavingStatus !=  eSaveStatus_Saving)	
		{
			for (; find_index < poolsize; find_index++)
			{
				//找一个需要保存的对象
				if(m_pObjList[find_index]->GetObjStatus() == eShmObj_Changed)
				{
					//拷贝对象
					pSavingObj->m_Obj = m_pObjList[find_index]->m_Obj;
					m_pObjList[find_index]->SetObjStatus(eShmObj_Saved);
					pSavingObj->m_nSavingIndex = find_index;
					pSavingObj->SetSavingStatus(eSaveStatus_Saving);
					break;
				}
			}
		}
		m_nSavingIndex = find_index + 1;
		
		//保存完晕了，重置存储定时器
		if(m_nSavingIndex == poolsize)
		{
			m_nSavingIndex = 0;
			m_bSaveAllFlag = 0;
			m_SaveTimer.ResetTimeout(nNow);
			break;
		}
	}
}

template<typename T,size_t poolsize,size_t saving_size>
void CShmPool<T,poolsize,saving_size>::DoSave()
{
    for(int saving_index = 0;saving_index < saving_size;saving_index++)
    {
        CSavingObj<T>* pSavingObj = m_SavingObjList[saving_index];
		try
        {
            bool nRet = pSavingObj->m_Obj.Save();
            if(nRet)
            {
                pSavingObj->SetSavingStatus(eSaveStatus_Saved);

            }else
            {
                pSavingObj->SetSavingStatus(eSaveStatus_Free);
            }
        }
        catch(const std::exception& e)
        {
            pSavingObj->SetSavingStatus(eSaveStatus_Free);
        }
    }
}
#endif //__SHM_POOL_H__