#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>
#include "base.h"

class CSpinLock
{
public:
	CSpinLock() : flag{ false }
	{
	}

	inline void Lock()
	{
		while (flag.test_and_set(std::memory_order_acquire));
	}

	inline bool TryLock()
	{
		return !flag.test_and_set(std::memory_order_acquire);
	}

	inline void UnLock()
	{
		flag.clear(std::memory_order_release);
	}
private:
	std::atomic_flag flag;
};

//自动加锁解锁器
class CSafeSpLock
{
public:
	CSafeSpLock() = delete;
	CSafeSpLock(CSpinLock& rLock)
	{
		m_pLock = &rLock;
		m_pLock->Lock();
	}

	~CSafeSpLock()
	{
		m_pLock->UnLock();
	}
private:
	CSpinLock* m_pLock;
};

class CSpinRWLock
{
public:
    CSpinRWLock() : state(0) {}

    inline void RLock()
    {
        uint32_t expected;
        do {
            expected = state.load(std::memory_order_relaxed);
            // 等待写锁释放且尝试增加读计数
        } while (expected & 0x80000000 || 
                !state.compare_exchange_weak(expected, 
                                           expected + 1,
                                           std::memory_order_acquire));
    }

    inline void WLock()
    {
        uint32_t expected = 0;
        // 自旋直到设置写标志位（最高位）
        while (!state.compare_exchange_weak(expected, 
                                          0x80000000,
                                          std::memory_order_acquire)) {
            expected = 0;
        }
        // 等待所有读锁释放
        while ((state.load(std::memory_order_acquire) & 0x7FFFFFFF) != 0);
    }

    inline void UnlockR()
    {
        state.fetch_sub(1, std::memory_order_release);
    }

    inline void UnlockW()
    {
        state.store(0, std::memory_order_release);
    }

private:
    CACHE_LINE_ALIGN size_t _;  // 填充缓存行，避免伪共享,避免splitlock
    std::atomic<uint32_t> state; // 最高位表示写锁，低31位表示读锁数量
};

//自动加锁解锁器
class CSafeSpinRLock
{
public:
    CSafeSpinRLock() = delete;
    CSafeSpinRLock(CSpinRWLock& rLock)
	{
		m_pLock = &rLock;
		m_pLock->RLock();
	}
	~CSafeSpinRLock()
	{
		m_pLock->UnlockR();
	}
private:
    CSpinRWLock* m_pLock;
};


//自动加锁解锁器
class CSafeSpinWLock
{
public:
    CSafeSpinWLock() = delete;
    CSafeSpinWLock(CSpinRWLock& rLock)
	{
		m_pLock = &rLock;
		m_pLock->WLock();
	}
	~CSafeSpinWLock()
	{
		m_pLock->UnlockW();
	}
private:
    CSpinRWLock* m_pLock;
};
#endif