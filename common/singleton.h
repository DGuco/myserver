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
#endif // __SERVER_TOOL_H__

