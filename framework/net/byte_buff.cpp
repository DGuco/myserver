//
// Created by dguco on 18-403.
//

#include <string>
#include <stdio.h>
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
	m_nReadIndex(0),
	m_nWriteIndex(0)
{
	m_aData = new BYTE[m_nCapacity + BUFF_EXTRA_SIZE]( );
	m_nCapacity = MAX_PACKAGE_LEN + BUFF_EXTRA_SIZE;
}

CByteBuff::CByteBuff(msize_t tmpCap)
	:
	m_nReadIndex(0),
	m_nWriteIndex(0)
{
	m_aData = new BYTE[tmpCap + BUFF_EXTRA_SIZE]( );
	m_nCapacity = tmpCap + BUFF_EXTRA_SIZE;
}

CByteBuff::CByteBuff(const CByteBuff &byteBuff)
{
	m_aData = new BYTE[m_nCapacity]( );
	Copy(byteBuff);
}

CByteBuff::CByteBuff(CByteBuff &&byteBuff)
	:
	m_nReadIndex(std::move(byteBuff.m_nReadIndex)),
	m_nWriteIndex(std::move(byteBuff.m_nWriteIndex)),
	m_nCapacity(std::move(byteBuff.m_nCapacity)),
	m_aData(std::move(byteBuff.m_aData))
{
	byteBuff.m_aData = NULL;
}

CByteBuff &CByteBuff::operator=(CByteBuff &byteBuff)
{
	if (this == &(byteBuff)) {
		return *this;
	}
	Copy(byteBuff);
	return *this;
}

CByteBuff &CByteBuff::operator=(CByteBuff &&byteBuff)
{
	if (this == &(byteBuff)) {
		return *this;
	}
	m_nReadIndex = std::move(byteBuff.m_nReadIndex);
	m_nWriteIndex = std::move(byteBuff.m_nWriteIndex);
	m_nCapacity = std::move(byteBuff.m_nCapacity);
	m_aData = std::move(byteBuff.m_aData);
	return *this;
}

CByteBuff::~CByteBuff()
{
	DELETE_ARR(m_aData);
}

void CByteBuff::Clear()
{
	m_nReadIndex = 0;
	m_nWriteIndex = 0;
	memset(m_aData, 0, m_nCapacity);
}

short CByteBuff::ReadShort(bool ispeek)
{
	return ReadT<short>(ispeek);
}

int CByteBuff::ReadInt(bool ispeek)
{
	return ReadT<int>(ispeek);
}

long CByteBuff::ReadLong(bool ispeek)
{
	return ReadT<long>(ispeek);
}

long long CByteBuff::ReadLongLong(bool ispeek)
{
	return ReadT<long long>(ispeek);
}

unsigned short CByteBuff::ReadUnShort(bool ispeek)
{
	return ReadT<unsigned short>(ispeek);
}

unsigned int CByteBuff::ReadUnInt(bool ispeek)
{
	return ReadT<unsigned int>(ispeek);
}

unsigned long CByteBuff::ReadUnLong(bool ispeek)
{
	return ReadT<unsigned long>(ispeek);
}

unsigned long long CByteBuff::ReadUnLongLong(bool ispeek)
{
	return ReadT<unsigned long long>(ispeek);
}

float CByteBuff::ReadFloat(bool ispeek)
{
	return ReadT<float>(ispeek);
}

double CByteBuff::ReadDouble(bool ispeek)
{
	return ReadT<double>(ispeek);
}

int CByteBuff::ReadBytes(BYTE* pOutCode, msize_t tmLen, bool ispeek)
{
	int nCanReadSpace = CanReadLen();
	if (nCanReadSpace <= 0 || tmLen > nCanReadSpace)
	{
		return -1;
	}

	msize_t usOutLength = tmLen;
	BYTE* pTempSrc = m_aData;
	BYTE* pTempDst = pOutCode;  // 设置接收 Code 的地址
	msize_t nReadLen = MIN(usOutLength, m_nCapacity - m_nReadIndex);
	memcpy((void*)pTempDst, (const void*)(pTempSrc + m_nReadIndex), nReadLen);
	msize_t tmpLast = usOutLength - nReadLen;
	if (tmpLast > 0)
	{
		memcpy((void*)(pTempDst + nReadLen), (const void*)pTempSrc, tmpLast);
	}
	if (!ispeek)
	{
		m_nReadIndex = (m_nReadIndex + usOutLength) % m_nCapacity;
		//m_nReadIndex = (m_nReadIndex + usOutLength) & (m_nCapacity - 1);
	}
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
	return m_aData;
}

int CByteBuff::WriteBytes(BYTE* pInCode, msize_t tmLen)
{
	msize_t nCanWriteSpace = CanWriteLen();
	//剩余空间不足
	if (tmLen > nCanWriteSpace)
	{
		return -1;
	}
	msize_t usInLength = tmLen;
	BYTE* pTempDst = m_aData;
	msize_t nWriteLen = MIN(usInLength, m_nCapacity - m_nWriteIndex);
	memcpy((void*)(pTempDst + m_nWriteIndex), (const void*)pInCode, (size_t)nWriteLen);
	size_t tmpLastLen = nCanWriteSpace - nWriteLen;
	//如果有剩余，说明空闲部分在内存的两头，在头部继续放
	if (tmpLastLen > 0)
	{
		memcpy((void*)pTempDst, (const void*)(pInCode + nWriteLen), tmpLastLen);
	}
}

template<class T,int len_>
T CByteBuff::ReadT(bool ispeek)
{
	BYTE tmpData[len_];
	ReadBytes(&tmpData[0], len_, ispeek);
	//因为不知道发送方是大端还是小端，所以默认发送方必须转换成大端发送(网络字节流默认以大端形式发送)，
	//如果本机是小端，接收到数据后把大端字节流转换成小端然后再使用
	/* plus
	  字符，字符串 都是以字符为单位的，所以读写数据时不会有大小端问题；
	  数值(short / int / float / double / ......)，有多个字符组成，在读写时会有大小端；
	  字符串可以理解为单字节的字符数组，字符之间没有直接关联，不存在字节序问题；
	*/
	if (IsLittleEndian( )) 
	{
		Reverse(tmpData, len_);
	}
	T result = *(T *) tmpData;
	return result;
}

template<class T, int len_>
void CByteBuff::WriteT(T t, int offset)
{
	BYTE tmpData[len_];
	*(T *) tmpData = t;
	BYTE* pSendStr = tmpData;
	//因为不知道接收方是大端还是小端，所以默认发送方必须转换成大端发送(网络字节流默认以大端形式发送)，
	//如果本机是小端，发送前把小端内存序转换为大端字网络流序再发送
	/* plus
	  字符，字符串 都是以字符为单位的，所以读写数据时不会有大小端问题；
	  数值(short / int / float / double / ......)，有多个字符组成，在读写时会有大小端；
	  字符串可以理解为单字节的字符数组，字符之间没有直接关联，不存在字节序问题；
	*/
	if (IsLittleEndian()) 
	{
		pSendStr = Reverse(&tmpData, len_);
	}
	WriteBytes(pSendStr, len_);
}

void CByteBuff::Copy(const CByteBuff& srcBuff)
{
	m_nReadIndex = srcBuff.m_nReadIndex;
	m_nWriteIndex = srcBuff.m_nWriteIndex;
	m_nCapacity = srcBuff.m_nCapacity;
	memcpy((void*)m_aData, (const void*)srcBuff.m_aData, m_nCapacity);
}

msize_t CByteBuff::GetReadIndex() const
{
	return m_nReadIndex;
}

msize_t CByteBuff::GetWriteIndex() const
{
	return m_nWriteIndex;
}

msize_t CByteBuff::GetCapaticy() const
{
	return m_nCapacity;
}

void CByteBuff::ResetReadIndex()
{
	m_nReadIndex = 0;
}

void CByteBuff::ResetWriteIndex()
{
	m_nWriteIndex = 0;
}

void CByteBuff::WriteLen(msize_t len)
{
	//m_nWriteIndex = (m_nWriteIndex + len) % m_nCapacity;
	m_nWriteIndex = (m_nWriteIndex + len) & (m_nCapacity - 1);
}

void CByteBuff::ReadLen(msize_t len)
{
	m_nReadIndex = (m_nReadIndex + len) % m_nCapacity;
	//m_nReadIndex = (m_nReadIndex + len) & (m_nCapacity - 1);
}

void CByteBuff::SetReadIndex(msize_t uiReadIndex)
{
	m_nReadIndex = uiReadIndex;
}

void CByteBuff::SetWriteIndex(msize_t uiWriteIndex)
{
	m_nWriteIndex = uiWriteIndex;
}

msize_t CByteBuff::CanReadLen() const
{
	if (m_nReadIndex == m_nWriteIndex) 
	{
		return 0;
	}
	else if (m_nReadIndex < m_nWriteIndex) 
	{
		//获取数据大小
		return (m_nWriteIndex - m_nReadIndex);
	}
	else 
	{
		return (m_nCapacity - (m_nReadIndex - m_nWriteIndex));
	}
}

msize_t CByteBuff::CanWriteLen() const
{
	msize_t nCanWriteSpace = 0;
	//获得剩余空间大小
	if (m_nReadIndex == m_nWriteIndex) 
	{
		nCanWriteSpace = m_nCapacity;
	}
	else if (m_nReadIndex > m_nWriteIndex)
	{
		nCanWriteSpace = m_nReadIndex - m_nWriteIndex;
	}
	else 
	{
		nCanWriteSpace = m_nCapacity - (m_nWriteIndex - m_nReadIndex);
	}

	/**
	 * 最大长度应该减去预留部分长度，保证首尾不会相接,
	 * 以此区分数据头不在共享内存区头部写满数据，和没有数据的情况
	 */
	nCanWriteSpace -= BUFF_EXTRA_SIZE;
	return nCanWriteSpace;
}

BYTE* CByteBuff::Flip(BYTE* netStr, size_t len)
{
	if (IsLittleEndian()) {
		Reverse(netStr, len);
	}
	return netStr;
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

int CByteBuff::Send(CSocket& socket)
{
	if (!socket.IsValid())
	{
		return -1;
	}
	msize_t nCanReadSpace = CanReadLen();
	//没有数据
	if (nCanReadSpace <= 0)
	{
		return 0;
	}
	
	SOCKET fd = socket.GetSocket();
	msize_t nByteLeft = MIN(nCanReadSpace, m_nCapacity - m_nReadIndex);
	BYTE* pByteSend = m_aData + m_nReadIndex;
	int nByteSent = 0;
	while (nByteLeft > 0)
	{
		nByteSent = send(fd, (const char*)(pByteSend), nByteLeft, 0);
		if (nByteSent > 0)
		{
			pByteSend += nByteSent;
			nByteLeft -= nByteLeft;
			m_nReadIndex = (m_nReadIndex + nByteSent) % m_nCapacity;
		}

		//发送出错
		if (nByteSent < 0)
		{
			if (errno != OPT_WOULD_BLOCK)
			{
				return -2;
			}
			else
			{
				return 0;  //下次继续尝试发送
			}
		}
	}

	nByteLeft = nCanReadSpace - nByteSent;
	//头部还有数据待发送
	if (nByteLeft > 0)
	{
		pByteSend = m_aData;
		while (nByteLeft > 0)
		{
			nByteSent = send(fd, (const char*)(pByteSend), nByteLeft, 0);
			if (nByteSent > 0)
			{
				pByteSend += nByteSent;
				nByteLeft -= nByteLeft;
				m_nReadIndex = (m_nReadIndex + nByteSent) % m_nCapacity;
			}

			//发送出错
			if (nByteSent < 0)
			{
				if (errno != OPT_WOULD_BLOCK)
				{
					return -1;
				}
				else
				{
					return 0;  //下次继续尝试发送
				}
			}
		}
	}
}

int CByteBuff::Recv(CSocket& socket)
{
	if (!socket.IsValid())
	{
		return -1;
	}

	msize_t nCanReadSpace = CanReadLen();
	//没有数据
	if (nCanReadSpace <= 0)
	{
		return -2;
	}

	SOCKET fd = socket.GetSocket();
	if (m_nReadIndex == m_nWriteIndex)
	{
		m_nReadIndex = 0;
		m_nWriteIndex = 0;
	}

	msize_t nCanReadLen = MIN(nCanReadSpace, m_nCapacity - m_nWriteIndex);
	int nByteRecved = 0;
	BYTE* pTempSrc = m_aData + m_nWriteIndex;
	int nReadLen = 0;
	do
	{
		nReadLen = recv(fd, (char*)pTempSrc,nCanReadLen, 0);
		if (nReadLen > 0)
		{
			nByteRecved += nReadLen;
			m_nWriteIndex = (m_nWriteIndex + nReadLen) % m_nCapacity;
			pTempSrc += nReadLen;
			//尾部没有空间了，剩下的放到数组头部接收
			if (nByteRecved >= nCanReadLen)
			{
				break;
			}
		}
		else if (nReadLen == 0)
		{
			return -3;
		}
		else
		{
			if (errno != EAGAIN)
			{
				return -4;
			}
			else
			{
				return 0;
			}
		}
	} while (nReadLen > 0);

	msize_t nLastBytes = nCanReadSpace - nByteRecved;
	if (nLastBytes > 0)
	{
		nByteRecved = 0;
		pTempSrc = m_aData;
		nReadLen = 0;
		do
		{
			nReadLen = recv(fd, (char*)pTempSrc, nLastBytes, 0);
			if (nReadLen > 0)
			{
				nByteRecved += nReadLen;
				m_nWriteIndex = (m_nWriteIndex + nReadLen) % m_nCapacity;
				pTempSrc += nReadLen;
				//没有空间用了
				if (nByteRecved >= nLastBytes)
				{
					return -5;
				}
			}
			else if (nReadLen == 0)
			{
				return -3;
			}
			else
			{
				if (errno != EAGAIN)
				{
					return -4;
				}
				else
				{
					return 0;
				}
			}
		} while (nReadLen > 0);
	}
	return 0;
}