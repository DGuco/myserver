#ifndef __DB_THREAD_H__
#define __DB_THREAD_H__

#include "my_thread.h"
#include "database_mysql.h"
#include "shm_pool.h"
#include "my_lock.h"
#include <queue>

class IDBThreadPool
{
public:
	virtual void Save(CSafePtr<IDataBase> pDatabase) = 0;
};

class CDBThread : public CMyThread
{
public:
	CDBThread(IDBThreadPool* pThreadPool);
	virtual ~CDBThread();
	virtual bool PrepareToRun();
	virtual bool PrepareEnd();
	virtual void Run();
private:
    CSafePtr<DatabaseMysql> m_pDatabase;
	CSafePtr<IDBThreadPool>	m_pThreadPool;
};

template<typename T>
class CDBThreadPool : public IDBThreadPool
{
public:
	CDBThreadPool();
	virtual ~CDBThreadPool();
	bool Init(size_t threads);
	virtual void Save(CSafePtr<IDataBase> pDatabase);
	void PushSavingObj(CSafePtr<CSavingObj<T>> pObj);
	void AddSaveShmType(enShmType shmType);
private:
	std::vector<CSafePtr<CMyThread>> m_Workers;
	std::vector<enShmType> m_SaveShmTypes;
	CMyLock		m_queue_mutex;
	std::queue<CSafePtr<CSavingObj<T>>> m_SavingQueue;
};

template<typename T>
CDBThreadPool<T>::CDBThreadPool()
{
	
}

template<typename T>
CDBThreadPool<T>::~CDBThreadPool()
{
	
}

template<typename T>
bool CDBThreadPool<T>::Init(size_t threads)
{
    time_t nNow = CTimeHelper::GetSingletonPtr()->GetMSTime();
	for (size_t i = 0; i < threads; ++i)
	{
		CSafePtr<CDBThread> pTaskThread = new CDBThread(this);
		pTaskThread->CreateThread();
		m_Workers.emplace_back(pTaskThread.DynamicCastTo<CMyThread>());
	}
	return true;
}

template<typename T>
void CDBThreadPool<T>::Save(CSafePtr<IDataBase> pDatabase)
{
	while (true)
	{
		CSafePtr<CSavingObj<T>> pSavingObj;
		{
			CSafeLock guard(m_queue_mutex);
			if (m_SavingQueue.empty())
			{
				break;
			}
			pSavingObj = this->m_SavingQueue.front();
			this->m_SavingQueue.pop();
		}
		if (pSavingObj != NULL)
		{
			try
			{
				pSavingObj->m_Obj.Save(pDatabase);
			}catch(...)
			{

			}
		}
	}
}

template<typename T>
void CDBThreadPool<T>::PushSavingObj(CSafePtr<CSavingObj<T>> pObj)
{
	CSafeLock guard(m_queue_mutex);
	m_SavingQueue.push(pObj);
}

template<typename T>
void CDBThreadPool<T>::AddSaveShmType(enShmType shmType)
{
	m_SaveShmTypes.push_back(shmType);
}

#endif