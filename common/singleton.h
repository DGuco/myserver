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
	static CSafePtr<T> GetSingletonPtr()
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
	time_t mNextTimeout;    // ��һ�γ�ʱʱ�䣨���룩
	time_t mDuration;        // ���γ�ʱ��������룩
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
	// ���뵱ǰʱ�䣨���룩�����Ƿ�ʱ�������ʱ��������һ�γ�ʱʱ�䣬������
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

	// ����timer��ʱʱ��
	void ResetTimeout(time_t tNow)
	{
		mNextTimeout = (tNow + mDuration);
	}
};
#endif // __SERVER_TOOL_H__

