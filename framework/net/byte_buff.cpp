//
// Created by dguco on 18-403.
//

#include <cstring>
#include "byte_buff.h"

bool IsSystemLittleEndian()
{
	union w
	{
		int a;
		char b;
	} w1;

	w1.a = 1;
	return (w1.b == 1);
}

bool CByteBuff::m_bIsLittleEndian = IsSystemLittleEndian( );

CByteBuff::CByteBuff()
	:
	m_uiReadIndex(0),
	m_uiWriteIndex(0),
	m_uiLen(0),
	m_uiCapacity(MAX_PACKAGE_LEN)
{
	m_acData = new BYTE[m_uiCapacity]( );
}

CByteBuff::CByteBuff(unsigned int tmpCap)
	:
	m_uiReadIndex(0),
	m_uiWriteIndex(0),
	m_uiLen(0),
	m_uiCapacity(tmpCap)
{
	m_acData = new BYTE[tmpCap]( );
}

CByteBuff::CByteBuff(BYTE*data, unsigned tmpCap)
{
	m_uiReadIndex = 0;
	m_uiWriteIndex = 0;
	m_uiLen = 0;
	m_uiCapacity = tmpCap;
	m_acData = new BYTE[tmpCap];
	memcpy(m_acData, data, tmpCap);
}

CByteBuff::CByteBuff(const CByteBuff &byteBuff)
{
	if (this == &(byteBuff)) {
		return;
	}
	m_acData = new BYTE[m_uiCapacity]( );
	Copy(&byteBuff);
}

CByteBuff::CByteBuff(CByteBuff &&byteBuff)
	:
	m_uiReadIndex(std::move(byteBuff.m_uiReadIndex)),
	m_uiWriteIndex(std::move(byteBuff.m_uiWriteIndex)),
	m_uiLen(std::move(byteBuff.m_uiLen)),
	m_uiCapacity(std::move(byteBuff.m_uiCapacity)),
	m_acData(std::move(byteBuff.m_acData))
{
	byteBuff.m_acData = NULL;
}

CByteBuff &CByteBuff::operator=(CByteBuff &byteBuff)
{
	if (this == &(byteBuff)) {
		return *this;
	}
	Copy(&byteBuff);
	return *this;
}

CByteBuff &CByteBuff::operator=(CByteBuff &&byteBuff)
{
	if (this == &(byteBuff)) {
		return *this;
	}
	m_uiReadIndex = std::move(byteBuff.m_uiReadIndex);
	m_uiWriteIndex = std::move(byteBuff.m_uiWriteIndex);
	m_uiLen = std::move(byteBuff.m_uiLen);
	m_uiCapacity = std::move(byteBuff.m_uiCapacity);
	m_acData = std::move(byteBuff.m_acData);
	return *this;
}

CByteBuff::~CByteBuff()
{
	DELETE_ARR(m_acData);
}

void CByteBuff::Clear()
{
	m_uiReadIndex = 0;
	m_uiWriteIndex = 0;
	m_uiLen = 0;
	memset(m_acData, 0, m_uiCapacity);
}

BYTE*CByteBuff::Flip(BYTE*netStr, size_t len)
{
	if (IsLittleEndian( )) {
		Reverse(netStr, len);
	}
	return netStr;
}

short CByteBuff::ReadShort()
{
	return ReadT<short>( );
}

int CByteBuff::ReadInt()
{
	return ReadT<int>( );
}

long CByteBuff::ReadLong()
{
	return ReadT<long>( );
}

long long CByteBuff::ReadLongLong()
{
	return ReadT<long long>( );
}

unsigned short CByteBuff::ReadUnShort()
{
	return ReadT<unsigned short>( );
}

unsigned int CByteBuff::ReadUnInt()
{
	return ReadT<unsigned int>( );
}

unsigned long CByteBuff::ReadUnLong()
{
	return ReadT<unsigned long>( );
}

unsigned long long CByteBuff::ReadUnLongLong()
{
	return ReadT<unsigned long long>( );
}

float CByteBuff::ReadFloat()
{
	return ReadT<float>( );
}

double CByteBuff::ReadDouble()
{
	return ReadT<double>( );
}

void CByteBuff::WriteShort(short value, int offset)
{
	WriteT<short>(value, offset);
}

void CByteBuff::WriteInt(int value, int offset)
{
	WriteT<int>(value, offset);
}

void CByteBuff::WriteLong(long value, int offset)
{
	WriteT<long>(value, offset);
}

void CByteBuff::WriteLongLong(long long value, int offset)
{
	WriteT<long long>(value, offset);
}

void CByteBuff::WriteUnShort(unsigned short value, int offset)
{
	WriteT<unsigned short>(value, offset);
}

void CByteBuff::WriteUnInt(unsigned int value, int offset)
{
	WriteT<unsigned short>(value, offset);
}

void CByteBuff::WriteUnLong(unsigned long value, int offset)
{
	WriteT<unsigned long>(value, offset);
}

void CByteBuff::WriteUnLongLong(unsigned long long value, int offset)
{
	WriteT<unsigned long long>(value, offset);
}

void CByteBuff::WriteFloat(float value, int offset)
{
	WriteT<float>(value, offset);
}

void CByteBuff::WriteDouble(double value, int offset)
{
	WriteT<double>(value, offset);
}

BYTE*CByteBuff::GetData() const
{
	return m_acData;
}

BYTE*CByteBuff::CanReadData() const
{
	return m_acData + m_uiReadIndex;
}

BYTE*CByteBuff::CanWriteData() const
{
	return m_acData + m_uiWriteIndex;
}

void CByteBuff::ReadBytes(BYTE*data, unsigned int len)
{
	memcpy(data, m_acData + m_uiReadIndex, len);
	m_uiReadIndex += len;
}

void CByteBuff::WriteBytes(BYTE*data, unsigned int len)
{
	memcpy(m_acData, data, len);
	m_uiWriteIndex += len;
}

template<class T>
T CByteBuff::ReadT()
{
	BYTE tmpData[64];
	size_t len = sizeof(T);
	memcpy((void *) tmpData, m_acData + m_uiReadIndex, len);
	//因为不知道发送方是大端还是小端，所以默认发送方必须转换成大端发送(网络字节流默认以大端形式发送)，
	//如果本机是小端，接收到数据后把大端字节流转换成小端然后再使用
	/* plus
	  字符，字符串 都是以字符为单位的，所以读写数据时不会有大小端问题；
	  数值(short / int / float / double / ......)，有多个字符组成，在读写时会有大小端；
	  字符串可以理解为单字节的字符数组，字符之间没有直接关联，不存在字节序问题；
	*/
	if (IsLittleEndian( )) 
	{
		Reverse(tmpData, len);
	}
	T result = *(T *) tmpData;
	m_uiReadIndex += len;
	return result;
}

template<class T>
void CByteBuff::WriteT(T t, int offset)
{
	BYTE tmpData[64];
	*(T *) tmpData = t;
	//因为不知道接收方是大端还是小端，所以默认发送方必须转换成大端发送(网络字节流默认以大端形式发送)，
	//如果本机是小端，发送前把小端内存序转换为大端字网络流序再发送
	/* plus
	  字符，字符串 都是以字符为单位的，所以读写数据时不会有大小端问题；
	  数值(short / int / float / double / ......)，有多个字符组成，在读写时会有大小端；
	  字符串可以理解为单字节的字符数组，字符之间没有直接关联，不存在字节序问题；
	*/
	BYTE* pSendStr = tmpData;
	if (IsLittleEndian()) 
	{
		pSendStr = Reverse(tmpData, len);
	}
	size_t len = sizeof(T);
	m_uiWriteIndex += offset;
	memcpy(m_acData + m_uiWriteIndex, pSendStr, len);
	m_uiWriteIndex += len;
}

void CByteBuff::Copy(const CByteBuff *srcBuff)
{
	m_uiReadIndex = srcBuff->m_uiReadIndex;
	m_uiWriteIndex = srcBuff->m_uiWriteIndex;
	m_uiLen = srcBuff->m_uiLen;
	m_uiCapacity = srcBuff->m_uiCapacity;
	memcpy(m_acData, srcBuff->m_acData, m_uiCapacity);
}

unsigned int CByteBuff::GetReadIndex() const
{
	return m_uiReadIndex;
}

unsigned int CByteBuff::GetWriteIndex() const
{
	return m_uiWriteIndex;
}

unsigned int CByteBuff::GetCapaticy() const
{
	return m_uiCapacity;
}

void CByteBuff::ResetReadIndex()
{
	m_uiReadIndex = 0;
}

void CByteBuff::ResetWriteIndex()
{
	m_uiWriteIndex = 0;
}

void CByteBuff::WriteLen(unsigned int len)
{
	m_uiWriteIndex += len;
}

void CByteBuff::ReadLen(unsigned int len)
{
	m_uiReadIndex += len;
}

void CByteBuff::SetReadIndex(unsigned int uiReadIndex)
{
	m_uiReadIndex = uiReadIndex;
}

void CByteBuff::SetWriteIndex(unsigned int uiWriteIndex)
{
	m_uiWriteIndex = uiWriteIndex;
}

unsigned int CByteBuff::WriteableDataLen() const
{
	return m_uiCapacity - m_uiWriteIndex;
}

unsigned int CByteBuff::ReadableDataLen() const
{
	return m_uiWriteIndex - m_uiReadIndex;
}

bool CByteBuff::IsLittleEndian()
{
	return m_bIsLittleEndian;
}

void CByteBuff::Reverse(BYTE*str, size_t len)
{
	BYTE *p = str + len - 1;
	BYTE temp;
	while (str < p) {
		temp = *p;
		*p-- = *str;
		*str++ = temp;
	}
}