/*****************************************************************
* FileName:my_lock.h
* Summary :
* Date	  :2024-7-12
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __MY_LOCK_H__
#define __MY_LOCK_H__

#include "base.h"
#include <mutex>

//共享锁
#if defined(__LINUX__)
class CMyLock
{
public:
	CMyLock() 
	{
		pthread_mutex_init(&m_Mutex, NULL); 
	}

	~CMyLock() 
	{
		pthread_mutex_destroy(&m_Mutex);
	}

	VOID	Lock() 
	{
		pthread_mutex_lock(&m_Mutex);
	}

	VOID	Unlock()
	{ 
		pthread_mutex_unlock(&m_Mutex); 
	}
private:
	pthread_mutex_t 	m_Mutex;
};

//自动加锁解锁器
class CSafeLock
{
public:
	CSafeLock() = delete;
	CSafeLock(CMyLock& rLock)
	{
		m_pLock = &rLock;
		m_pLock->Lock();
	}
	~CSafeLock()
	{
		m_pLock->Unlock();
	}
private:
	CMyLock* m_pLock;
};

class CMyRWLock
{
public:
	CMyLock()
	{
		pthread_rwlock_init(&m_Mutex, NULL);
	}

	~CMyLock()
	{
		pthread_rwlock_destroy(&m_Mutex);
	}

	void	RLock()
	{
		pthread_rwlock_rdlock(&m_Mutex);
	}

	void	WLock()
	{
		pthread_rwlock_wrlock(&m_Mutex);
	}

	void	Unlock()
	{
		pthread_rwlock_unlock(&m_Mutex);
	}
private:
	pthread_rwlock_t  	m_Mutex;
};

//自动加锁解锁器
class CSafeRLock
{
public:
	CSafeRLock() = delete;
	CSafeRLock(CMyRWLock& rLock)
	{
		m_pLock = &rLock;
		m_pLock->RLock();
	}
	~CSafeRLock()
	{
		m_pLock->Unlock();
	}
private:
	CMyRWLock* m_pLock;
};

//自动加锁解锁器
class CSafeWLock
{
public:
	CSafeWLock() = delete;
	CSafeWLock(CMyRWLock& rLock)
	{
		m_pLock = &rLock;
		m_pLock->WLock();
	}
	~CSafeRLock()
	{
		m_pLock->Unlock();
	}
private:
	CMyRWLock* m_pLock;
};
#else
	#define CMyLock std::mutex 
	#define CSafeLock std::lock_guard<std::mutex>
	#define CSafeRLock std::lock_guard<std::mutex>
	#define CSafeWLock std::lock_guard<std::mutex>
#endif

#endif //__MY_LOCK_H__
