//
// Created by DGcuo on 17-7-5.
//

#ifndef __SERVER_TOOL_H__
#define __SERVER_TOOL_H__

#include <vector>
#include <string>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <memory>

#include "safe_pointer.h"

using namespace std;

// ********************************************************************** //
// CSingleton
// ********************************************************************** //
template<typename T>
class CSingleton
{
public:
	~CSingleton() = default;
	CSingleton() = default;
private:
	CSingleton(const CSingleton &temp) = delete;
	CSingleton &operator=(const CSingleton &temp) = delete;
public:
	static SafePointer<T> GetSingletonPtr()
	{
		static T intance;
		return &intance;
	}
};

// ********************************************************************** //
// CWTimer
// ********************************************************************** //
class CWTimer
{
protected:
	time_t mNextTimeout;    // 下一次超时时间（毫秒）
	time_t mDuration;        // 单次超时间隔（毫秒）
public:
	CWTimer()
	{
		mDuration = 0;
		mNextTimeout = 0;
	}

	void Initialize(time_t vDuration)
	{
		mDuration = vDuration;
		mNextTimeout = (time(NULL) + mDuration);
	}

public:
	// 传入当前时间（毫秒）返回是否超时，如果超时将计算下一次超时时间，并补偿
	inline bool IsTimeout(time_t tNow)
	{
		if (mDuration <= 0) {
			return false;
		}

		if (tNow >= mNextTimeout) {
			if (mNextTimeout == 0) {
				mNextTimeout = (time(NULL) + mDuration);
			}
			else {
				mNextTimeout += mDuration;
			}

			return true;
		}

		return false;
	}

	// 重置timer超时时间
	void ResetTimeout(time_t tNow)
	{
		mNextTimeout = (tNow + mDuration);
	}
};

// ********************************************************************** //
// 双向链表基类
// ********************************************************************** //
class CDoubleLinker
{
public:
	CDoubleLinker()
	{
		Clear();
	}

	~CDoubleLinker()
	{}

	int Init()
	{
		Clear();

		return 0;
	}

	int Resume()
	{
		return 0;
	}

	void Clear()
	{
		mpNext = NULL;
		mpPrev = NULL;
	}

	void SetNext(CDoubleLinker *pPtr)
	{ mpNext = pPtr; }

	CDoubleLinker *GetNext()
	{ return mpNext; }

	void SetPrev(CDoubleLinker *pPtr)
	{ mpPrev = pPtr; }

	CDoubleLinker *GetPrev()
	{ return mpPrev; }

	// 是否在链表中
	bool InLinker()
	{ return (mpPrev == NULL && mpNext == NULL) ? false : true; }

protected:
	CDoubleLinker *mpNext;
	CDoubleLinker *mpPrev;
};

// ********************************************************************** //
// 双向链表头尾信息
// ********************************************************************** //
class CDoubleLinkerInfo
{
public:
	CDoubleLinkerInfo()
	{
		Clear();
	}

	CDoubleLinkerInfo(const CDoubleLinkerInfo &info)
	{
		(*this) = info;
	}

	~CDoubleLinkerInfo()
	{}

	int Initialize()
	{
		Clear();

		return 0;
	}

	int Resume()
	{
		return 0;
	}

	void Clear()
	{
		ClearHead();
		ClearEnd();
		miCnt = 0;
	}

	bool operator==(const CDoubleLinkerInfo &__x) const;

	bool operator!=(const CDoubleLinkerInfo &__x) const;

	CDoubleLinkerInfo &operator=(const CDoubleLinkerInfo &__x);

	// 将结点插入链表
	void insert(CDoubleLinker *pNode);

	// 从链表中删除该结点
	// 这个函数应该是不安全的，但为了效率，所以这样处理
	// 如果该node不是这个链表中的元素，这里也会处理，当该元素是其他链表的中间元素，则不会出任何问题
	// 如果该node不是其他链表的中间元素，那么那个链表将被破坏，而且下面会崩溃，因为prev或next其中一个肯定为空
	// 所以在外层使用的是有一定得注意
	int erase(CDoubleLinker *pNode);

	void SetHead(CDoubleLinker *pPtr)
	{ mpHead = pPtr; }

	CDoubleLinker *GetHead()
	{ return mpHead; }

	void ClearHead()
	{ mpHead = NULL; }

	void SetEnd(CDoubleLinker *pPtr)
	{ mpEnd = pPtr; }

	CDoubleLinker *GetEnd()
	{ return mpEnd; }

	void ClearEnd()
	{ mpEnd = NULL; }

	int GetCnt()
	{ return miCnt; }

protected:
	CDoubleLinker *mpHead;
	CDoubleLinker *mpEnd;
	int miCnt;    // 链表中元素数量
};


#endif // __SERVER_TOOL_H__

