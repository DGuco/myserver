/*****************************************************************
* FileName:t_array.h
* Summary :
* Date	  :2023-8-3
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __T_ARRAY_H__
#define __T_ARRAY_H__

#include "my_assert.h"
#include <type_trais>

template<typename Type_,int Len_>
class TArray
{
	static_assert(std::is_standard_layout<Class_>, "TArray not support not stand layout class");
	TArray()
	{
		memset(m_data, 0, sizeof(Type_) * Len_);
	}

	Type_& operator[] (int index)
	{
		MY_ASSERT(index >= 0 && index < Len_, "index is out of range");
		return m_data[index];
	}

	const Type_& operator[] (int index)
	{
		MY_ASSERT(index >= 0 && index < Len_, "index is out of range");
		return m_data[index];
	}
private:
	Type_ m_data[Len_];
};
#endif //__T_ARRAY_H__
