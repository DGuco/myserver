/*****************************************************************
* FileName:t_array.h
* Summary :
* Date	  :2023-8-3
* Author  :DGuco(1139140929@qq.com)
******************************************************************/
#ifndef __T_ARRAY_H__
#define __T_ARRAY_H__

#include <string>
#include <type_traits>
#include "my_assert.h"

using namespace std;

namespace my_std
{
	template<typename Type_, int Len_>
	class TArray
	{
	public:
		static_assert(std::is_standard_layout<Type_>::value, "TArray not support not stand layout class");
		TArray()
		{
			memset(m_data, 0, sizeof(Type_) * Len_);
		}

		Type_& operator[] (int index)
		{
			ASSERT_EX(index >= 0 && index < Len_, "index is out of range");
			return m_data[index];
		}

		const Type_& operator[] (int index) const
		{
			ASSERT_EX(index >= 0 && index < Len_, "index is out of range");
			return m_data[index];
		}

		const Type_* GetData() const
		{
			return m_data;
		}

	protected:
		Type_ m_data[Len_];
	};
}
#endif //__T_ARRAY_H__
