#ifndef C_STRING_H
#define C_STRING_H

#include "t_array.h"

namespace my_std
{
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
			strncpy(TArray<char, Len_>::m_data, str.c_str(), Len_);
			TArray<char, Len_>::m_data[Len_ - 1] = '\0';
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

			return std::string(TArray<char, Len_>::m_data) == std::string(pstr);
		}

		template<int LenFrom_>
		bool operator==(std::string str)
		{
			return std::string(TArray<char, Len_>::m_data) == str;
		}

		const char* c_str() const
		{
			return TArray<char, Len_>::m_data;
		}
	};

}

#endif