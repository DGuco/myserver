//
// Created by DGcuo on 17-7-5.
//

#ifndef __SINGLE_TOOL_H__
#define __SINGLE_TOOL_H__

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
	//次实现，多线程下的初始化只限于c++11以上的编译器保证完全正确
	/*静态局部变量的线程安全性当一个函数被多个线程调用时，如果该函数内部包含静态局部变量，C++11标准确保了以下几点：
	   1首次访问线程安全：当多个线程首次访问该静态局部变量时，它们会并发地执行初始化代码。但是，每个线程看到的初始化结果是一致的，
	即只有一个线程会执行初始化代码。
	   2 只初始化一次：无论有多少个线程尝试访问该静态局部变量，它只会被初始化一次。这是通过某种形式的内部同步（如互斥锁）来保证的，
	   但这个细节是由编译器实现的，对用户是透明的。
	*/
		static T intance;
		return &intance;
	}
};
#endif // __SINGLE_TOOL_H__

