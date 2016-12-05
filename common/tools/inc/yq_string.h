/*!
 * \file cstring.h
 *
 * Copyright (c) 2002, Asiainfo Technologies (China), Inc.
 * All right reserved
 *
 * \brief string.
 *
 * \author
 *
 * \version 2.0
 * \date 2002/11/15
 */

#ifndef _CSTRING_H_
#define _CSTRING_H_

#include <stdarg.h>
#include <ostream>

namespace yq 
{

template <int size>
class string
{
private:
	enum 		{MAX_SIZE = size};
	
	char		m_value[MAX_SIZE + 1];
	short		m_len;
	
public:
	string() {/*m_len = 0;*/}
	~string() {}
	
	string(const string& value)
	{
		strcpy(m_value, value.m_value);
		m_len = value.m_len;
	}
	
	string(const char* value)
	{
		strncpy(m_value, value, MAX_SIZE);
		m_value[MAX_SIZE] = '\0';
		m_len = strlen(m_value);
	}
	
	string(char value)
	{
		m_value[0] = value;
		m_value[1] = 0;
		m_len = strlen(m_value);
	}
	
	inline short length() {return m_len;}
	
	inline const char* c_str() const 
	{
		return m_value;
	}

	inline 
	string& append(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		int n = vsnprintf(&m_value[m_len], MAX_SIZE - m_len, fmt, ap);
		va_end(ap);

		if ( n >= (MAX_SIZE - m_len) )
		{
				m_len = MAX_SIZE - 1;
		}else
		{
			m_len += n;
		}

		return *this;
	}
	
	inline
	string& append(const string& value)
	{
		strncat(m_value, value.m_value, MAX_SIZE - m_len);
		m_len = strlen(m_value);
		
		return *this;
	}
	
	/*
	inline
	string& append(const char* value)
	{
		strncat(m_value, value, MAX_SIZE - m_len);
		m_len = strlen(m_value);
		
		return *this;
	}
	*/

	inline
	string& append(char ch)
	{
		if(m_len < MAX_SIZE)
		{
			m_value[m_len++] = ch;
			m_value[m_len] = 0;
		}
		return *this;
	}

	inline
	void clear()
	{
		m_value[0] = 0;
		m_len = 0;
	}

	friend std::ostream& operator << (std::ostream& os, string& s)
	{
		os << s.m_value;
		return os;
	}
	
	inline friend
	string operator + (const string& lhs, const string& rhs)
	{
		return string(lhs).append(rhs);
	}
	
	inline friend
	string operator + (const char* lhs, const string& rhs)
	{
		return string(lhs).append(rhs);
	}
	
	inline friend
	string operator + (const string& lhs, const char* rhs)
	{
		return string(lhs).append(rhs);
	}
	
	inline friend
	string operator + (char lhs, const string& rhs)
	{
		return string(lhs).append(rhs);
	}
	
	inline friend
	string operator + (const string& lhs, char rhs)
	{
		return string(lhs).append(string(rhs));
	}

	inline
	string& operator += (const string& value)
	{
		return append(value);
	}
	
	inline
	string& operator += (const char* value)
	{
		return append(value);
	}
	
	inline
	char operator [] (short index)
	{
		if (index <= m_len)	return m_value[index];

		return 0;
	}

	inline
	string& operator = (const string& value)
	{
		if (this != &value)
		{
			strcpy(m_value, value.m_value);
			m_len = value.m_len;
		}
		
		return *this;
	}
	
	inline
	bool operator == (const string& value) const
	{
		if (this != &value)
		{
			if (m_len != value.m_len) return false;
			return (strcmp(m_value, value.m_value) == 0);
		}
		
		return true;
	}
	
	inline
	bool operator < (const string& value) const
	{
		if (this != &value)
		{
			if (m_len < value.m_len) return true;
			if (m_len > value.m_len) return false;
			return (strcmp(m_value, value.m_value) < 0);
		}
		
		return false;
	}
	
	inline
	bool operator > (const string& value) const
	{
		if (this != &value)
		{
			if (m_len > value.m_len) return true;
			if (m_len < value.m_len) return false;
			return (strcmp(m_value, value.m_value) > 0);
		}
		
		return false;
	}
};

typedef string<8>		CString8;
typedef string<16>		CString16;
typedef string<32>		CString32;
typedef string<64>		CString64;
typedef string<128>		CString128;

};

#endif
