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
#include "time_helper.h"
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
	std::atomic<byte>	m_bStatus;
	T					m_Obj;
	//对原子变量y进行acquire的load操作，因此变量y之后的store/load操作不能排序到y之前
	ShmObjStatus GetObjStatus()						{ return m_bStatus.load(std::memory_order_acquire); }
	//对原子变量y进行了release的store操作，因此y变量之前的store/load操作不能排序到y之后
	void SetObjStatus(ShmObjStatus state)			{ m_bStatus.store(m_bStatus, std::memory_order_release);}

	CShmObj()
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
	T					m_SavingObj;      //保存对象
	int					m_nSavingIndex;   //保存对象索引
	std::atomic<byte>	m_bSavingStatus;  //保存对象状态
	//对原子变量y进行acquire的load操作，因此变量y之后的store/load操作不能排序到y之前
	SavingStauts GetSavingStatus()						{ return m_bSavingStatus.load(std::memory_order_acquire); }
	//对原子变量y进行了release的store操作，因此y变量之前的store/load操作不能排序到y之后
	void SetSavingStatus(SavingStauts state)			{ m_bSavingStatus.store(m_bSavingStatus, std::memory_order_release);}
};

class IShmPool
{
public:
	virtual void PrepareSave() = 0;
	virtual void DoSaveAll() = 0;
};

template<typename T,size_t saving_size = 1>
class CShmPool : public IShmPool
{
public:
	CShmPool(int saveInterval,bool forceSaveAll);
	virtual ~CShmPool();
	//
	bool Init(int poolkey, int poolsize);
	//
	CSafePtr<CShmObj<T>> NewObj();
	//
	CSafePtr<CShmObj<T>> GetObj(int index);
	//
	int GetMaxSize();
	//
	virtual void PrepareSave();
	//
	virtual void DoSaveAll();
private:
	CShmObj<T>**			m_pObjList;
	int 					m_nSaveInterval; //保存间隔时间
	CMyTimer				m_SaveTimer;  //保存定时器
	byte					m_bSaveAllFlag;	//保存所有对象的状态
	bool					m_bForceSaveAll;
	int						m_nSavingIndex;   //保存对象索引
	CSavingObj<T>			m_SavingObjList[saving_size];
	CSharedMem				m_ShareMem;
	int						m_nMaxSize;
	int						m_nUsedIndex;
};

template<typename T,size_t saving_size>
CShmPool<T,saving_size>::CShmPool(int saveInterval,bool forceSaveAll)
{
	m_nMaxSize = 0;
	m_nUsedIndex = 0;
	m_nSavingIndex = 0;
	m_bForceSaveAll = forceSaveAll;
	m_nSaveInterval = saveInterval;
	m_bSaveAllFlag = 0;	
	m_pObjList = NULL;
}

template<typename T,size_t saving_size>
CShmPool<T,saving_size>::~CShmPool()
{

}

template<typename T,size_t saving_size>
bool CShmPool<T,saving_size>::Init(int poolkey, int poolsize)
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
	m_SaveTimer.BeginTimer(m_nSaveInterval);
	return true;
}

template<typename T,size_t saving_size>
CSafePtr<CShmObj<T>> CShmPool<T,saving_size>::NewObj()
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


template<typename T,size_t saving_size>
CSafePtr<CShmObj<T>> CShmPool<T,saving_size>::GetObj(int index)
{
	if (index >= 0 && index < m_nMaxSize)
	{
		return m_pObjList[index];
	}
	return NULL;
}

template<typename T,size_t saving_size>
int CShmPool<T,saving_size>::GetMaxSize()
{
	return m_nMaxSize;
}

template<typename T,size_t saving_size>
void CShmPool<T,saving_size>::PrepareSave()
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
		CSavingObj<T>& tmSavingObj = m_SavingObjList[index];
		size_t find_index = m_nSavingIndex;
		if (tmSavingObj.m_bSavingStatus !=  eSaveStatus_Saving)	
		{
			for (; find_index < m_nMaxSize; find_index++)
			{
				//找一个需要保存的对象
				if(m_pObjList[find_index]->GetObjStatus() == eShmObj_Changed)
				{
					//拷贝对象
					tmSavingObj.m_SavingObj = m_pObjList[find_index]->m_Obj;
					m_pObjList[find_index]->SetObjStatus(eShmObj_Saved);
					tmSavingObj.m_nSavingIndex = find_index;
					tmSavingObj.SetSavingStatus(eSaveStatus_Saving);
					break;
				}
			}
		}
		m_nSavingIndex = find_index + 1;

		//保存完晕了，重置存储定时器
		if(m_nSavingIndex == m_nMaxSize)
		{
			m_nSavingIndex = 0;
			m_bSaveAllFlag = 0;
			m_SaveTimer.ResetTimeout();
			break;
		}
	}
}

template<typename T,size_t saving_size>
void CShmPool<T,saving_size>::DoSaveAll()
{
	if(GetSavingStatus() == eSaveStatus_Saving)
	{
		if(m_nSavingIndex >= 0 && m_nSavingIndex < m_nMaxSize)
		{
			try
			{
				bool nRet = m_SavingObj.Save();
				if(nRet)
				{
					SetSavingStatus(eSaveStatus_Saved)
				}else
				{
					SetSavingStatus(eSaveStatus_Free);
				}
			}
			catch(const std::exception& e)
			{
				SetSavingStatus(eSaveStatus_Free)
			}
		}
	}
}

#endif //__SHM_POOL_H__