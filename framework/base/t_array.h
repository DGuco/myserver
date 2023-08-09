/*****************************************************************
* FileName:t_array.h
* Summary :
* Date	  :2023-8-3
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __T_ARRAY_H__
#define __T_ARRAY_H__

#include "my_assert.h"
#include <string>
#include <type_traits>

using namespace std;

namespace my_std
{
	template<typename Type_, int Len_>
	class TArray
	{
		static_assert(std::is_standard_layout<Type_>::value, "TArray not support not stand layout class");
		TArray()
		{
			memset(m_data, 0, sizeof(Type_) * Len_);
		}

		Type_& operator[] (int index)
		{
			ASSERT(index >= 0 && index < Len_, "index is out of range");
			return m_data[index];
		}

		const Type_& operator[] (int index) const
		{
			ASSERT(index >= 0 && index < Len_, "index is out of range");
			return m_data[index];
		}

		const Type_* GetData() const
		{
			return m_data;
		}

	private:
		Type_ m_data[Len_];
	};

	template<int Len_>
	class CString : public TArray<char, Len_>
	{
	public:
		CString()
		{}

		CString(const char* pstr)
		{
			if (pstr != NULL)
			{
				strncpy(TArray<char, Len_>::m_data, pstr, Len_);
				TArray<char, Len_>::m_data[Len_ - 1] = '\0';
			}
		}

		CString(std::string str)
		{
			if (pstr != NULL)
			{
				strncpy(TArray<char, Len_>::m_data, str.c_str(), Len_);
				TArray<char, Len_>::m_data[Len_ - 1] = '\0';
			}
		}

		template<int LenFrom_>
		CString(CString<LenFrom_> other)
		{
			strncpy(TArray<char, Len_>::m_data, other.GetData(), std::min(Len_, LenFrom_));
			TArray<char, Len_>::m_data[Len_ - 1] = '\0';
		}

		CString<Len_>& operator=(CString<Len_>& other)
		{
			if (this == &other)
			{
				return *this;
			}
			memset(TArray<char, Len_>::m_data, 0, sizeof(char) * Len_);
			strncpy(TArray<char, Len_>::m_data, other.GetData(), Len_);
			TArray<char, Len_>::m_data[Len_ - 1] = '\0';
			return *this;
		}

		template<int LenFrom_>
		CString<Len_>& operator=(CString<LenFrom_>& other)
		{
			memset(TArray<char, Len_>::m_data, 0, sizeof(char) * Len_);
			strncpy(TArray<char, Len_>::m_data, other.GetData(), std::min(Len_, LenFrom_));
			TArray<char, Len_>::m_data[Len_ - 1] = '\0';
			return *this;
		}

		CString<Len_>& operator=(const char* pstr)
		{
			memset(TArray<char, Len_>::m_data, 0, sizeof(char) * Len_);
			if (pstr != NULL)
			{
				strncpy(TArray<char, Len_>::m_data, pstr, Len_);
				TArray<char, Len_>::m_data[Len_ - 1] = '\0';
			}

			return *this;
		}

		template<int LenFrom_>
		bool operator==(CString<LenFrom_>& other)
		{
			return std::string(TArray<char, Len_>::m_data) == std::string(other.GetData());
		}

		template<int LenFrom_>
		bool operator==(const char* pstr)
		{

			return std::string(TArray<char, Len_>::m_data) == std::string(other.GetData());
		}

		template<int LenFrom_>
		bool operator==(std::string str)
		{

			return std::string(TArray<char, Len_>::m_data) == str;
		}

		const char* c_str() const
		{
			return m_data;
		}
	};
}
#endif //__T_ARRAY_H__
