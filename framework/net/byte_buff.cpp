//
// Created by dguco on 18-4-13.
//

#include <cstring>
#include "byte_buff.h"

int IsLittleEndian()
{
	union w
	{
		int a;
		char b;
	} w1;

	w1.a = 1;
	return (w1.b == 1);
}

bool CByteBuff::m_bIsLittleEndian = IsLittleEndian();

CByteBuff::CByteBuff()
	:
	m_iReadIndex(0),
	m_iWriteIndex(0),
	m_iLen(0)
{
	memset(m_acData, 0, sizeof(m_acData));
}

bool CByteBuff::IsLittleEndian()
{
	return m_bIsLittleEndian;
}
