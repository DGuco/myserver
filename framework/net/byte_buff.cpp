//
// Created by dguco on 18-403.
//

#include <string>
#include <stdio.h>
#include "byte_buff.h"
#include "tcp_def.h"

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
	m_nCapacity = 0;
	m_aData = new BYTE[m_nCapacity];
	m_nMinSize = 0;
	m_nMaxSize = 0;
	m_fBuffUseRate = 0.0f;
}

CByteBuff::CByteBuff(int minsize,int maxsize)
	:
	m_nReadIndex(0),
	m_nWriteIndex(0)
{
	m_nCapacity = minsize;
	m_nMinSize = minsize;
	m_fBuffUseRate = 0.0f;
	m_nMaxSize = MAX(maxsize,minsize);
	m_aData = new BYTE[m_nCapacity];
}

CByteBuff::CByteBuff(const CByteBuff &byteBuff)
{
	m_aData = new BYTE[m_nCapacity];
	Copy(byteBuff);
}

CByteBuff &CByteBuff::operator=(CByteBuff &byteBuff)
{
	if (this == &(byteBuff)) {
		return *this;
	}
	Copy(byteBuff);
	return *this;
}

CByteBuff::~CByteBuff()
{
	DELETE_ARR(m_aData);
	m_nCapacity = 0;
	m_nMinSize = 0;
	m_nMaxSize = 0;
	m_nReadIndex = 0;
	m_nWriteIndex = 0;
}

void CByteBuff::Clear()
{
	m_nReadIndex = 0;
	m_nWriteIndex = 0;
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

int CByteBuff::ReadBytes(BYTE* pOutCode, int tmLen, bool ispeek)
{
	int nCanReadSpace = CanReadLen();
	if (nCanReadSpace <= 0 || tmLen > nCanReadSpace)
	{
		return -1;
	}

	int usOutLength = tmLen;
	BYTE* pTempSrc = m_aData;
	BYTE* pTempDst = pOutCode;  // 设置接收 Code 的地址
	int nReadLen = MIN(usOutLength, m_nCapacity - m_nReadIndex);
	memcpy((void*)pTempDst, (const void*)(pTempSrc + m_nReadIndex), nReadLen);
	int tmpLast = usOutLength - nReadLen;
	//如果还有数据则去头部继续读取
	if (tmpLast > 0)
	{
		memcpy((void*)(pTempDst + nReadLen), (const void*)pTempSrc, tmpLast);
	}
	if (!ispeek)
	{
		m_nReadIndex = (m_nReadIndex + usOutLength) % m_nCapacity;
		//m_nReadIndex = (m_nReadIndex + usOutLength) & (m_nCapacity - 1);
	}
	return 0;
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

BYTE* CByteBuff::GetData() const
{
	return m_aData;
}

int CByteBuff::WriteBytes(BYTE* pInCode, int tmLen)
{
	int nCanWriteSpace = CanWriteLen();
	//剩余空间必须大于写入的数据长度，否则会头尾相连，无法区分缓冲区满了还是缓冲区是空的
	if (tmLen >= nCanWriteSpace)
	{
		if (m_nCapacity < m_nMaxSize)
		{
			GrowBuff(tmLen - nCanWriteSpace);
			m_fBuffUseRate = CaclUseRate();
			nCanWriteSpace = CanWriteLen();
			if (tmLen >= nCanWriteSpace)//剩余空间不足
			{
				InitBuff();
				return -1;
			}
		}
		else //剩余空间不足
		{
			InitBuff();
			return -1;
		}
	}
	int usInLength = tmLen;
	BYTE* pTempDst = m_aData;
	int nWriteLen = MIN(usInLength, m_nCapacity - m_nWriteIndex);
	memcpy((void*)(pTempDst + m_nWriteIndex), (const void*)pInCode, (size_t)nWriteLen);
	int tmpLastLen = usInLength - nWriteLen;
	//如果有剩余，说明空闲部分在内存的两头，在头部继续放
	if (tmpLastLen > 0)
	{
		memcpy((void*)pTempDst, (const void*)(pInCode + nWriteLen), tmpLastLen);
	}
	m_nWriteIndex = (m_nWriteIndex + usInLength) % m_nCapacity;
	return 0;
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
		Reverse(pSendStr, len_);
	}
	WriteBytes(pSendStr, len_);
}

void CByteBuff::Copy(const CByteBuff& srcBuff)
{
	m_nCapacity = srcBuff.m_nCapacity;
	m_nMinSize = srcBuff.m_nMinSize;
	m_nMaxSize = srcBuff.m_nMaxSize;
	m_nReadIndex = srcBuff.m_nReadIndex;
	m_nWriteIndex = srcBuff.m_nWriteIndex;
	m_fBuffUseRate = srcBuff.m_fBuffUseRate;
	m_ResizeTimer = srcBuff.m_ResizeTimer;
	memcpy((void*)m_aData, (const void*)srcBuff.m_aData, m_nCapacity);
}
//计算利用率
float CByteBuff::CaclUseRate()
{
	//统计利用率
	float nCap = m_nCapacity;
	float nDataLen = CanReadLen();
	float fUseRate = nDataLen / nCap;
	return fUseRate;
}

int CByteBuff::GetReadIndex() const
{
	return m_nReadIndex;
}

int CByteBuff::GetWriteIndex() const
{
	return m_nWriteIndex;
}

int CByteBuff::GetCapaticy() const
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

void CByteBuff::WriteLen(int len)
{
	m_nWriteIndex = (m_nWriteIndex + len) % m_nCapacity;
	//m_nWriteIndex = (m_nWriteIndex + len) & (m_nCapacity - 1);
}

void CByteBuff::ReadLen(int len)
{
	m_nReadIndex = (m_nReadIndex + len) % m_nCapacity;
	//m_nReadIndex = (m_nReadIndex + len) & (m_nCapacity - 1);
}

void CByteBuff::SetReadIndex(int uiReadIndex)
{
	m_nReadIndex = uiReadIndex;
}

void CByteBuff::SetWriteIndex(int uiWriteIndex)
{
	m_nWriteIndex = uiWriteIndex;
}

int CByteBuff::CanReadLen() const
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

int CByteBuff::CanWriteLen() const
{
	int nCanWriteSpace = 0;
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
		return ERR_SEND_NOSOCK;
	}
	int nCanReadSpace = CanReadLen();
	//没有数据
	if (nCanReadSpace <= 0)
	{
		return ERR_SEND_OK;
	}
	
	SOCKET fd = socket.GetSocket();
	//数据可能在中间也可能在两边，先发送尾部的数据
	int nByteLeft = MIN(nCanReadSpace, m_nCapacity - m_nReadIndex);
	BYTE* pByteSend = m_aData + m_nReadIndex;
	int nByteSent = 0;
	while (nByteLeft > 0)
	{
		nByteSent = send(fd, (const char*)(pByteSend), nByteLeft, 0);
		if (nByteSent > 0)
		{
			pByteSend += nByteSent;
			nByteLeft -= nByteSent;
			m_nReadIndex = (m_nReadIndex + nByteSent) % m_nCapacity;
		}

		//发送出错
		if (nByteSent < 0)
		{
			if (errno != OPT_WOULD_BLOCK)
			{
				return ERR_SEND_SOCKET_ERROR;
			}
			else
			{
				return ERR_SEND_WOULD_BLOCK;  //下次继续尝试发送
			}
		}

		//下次继续尝试发送
		if (nByteSent == 0)
		{
			return ERR_SEND_OK;
		}
	}

	nCanReadSpace = CanReadLen();
	//发送完了
	if (nCanReadSpace <= 0)
	{
		return ERR_SEND_OK;
	}

	/*如果数据在中间，第一次没有全部发送出去，说明网络阻塞了，无法继续发送，直接返回等待下次发送
	正常应该不会走到这里，无法继续发送send返回值会返回0或者返回OPT_WOULD_BLOCK，上面会直接返回
	不会走到这里,如果数据在两边，尾部发送完成后，m_nReadIndex肯定等于0，如果不等于0，说明上面的
	尾部数据发送逻辑，没有发送完，网络阻塞了，不再继续发送，下次继续尝试发送
	*/
	if (m_nReadIndex != 0)
	{
		return ERR_SEND_OK;
	}

	//继续发送缓冲区头部的数据
	nByteLeft = MIN(nCanReadSpace, m_nCapacity - m_nReadIndex);
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
				return ERR_SEND_SOCKET_ERROR;
			}
			else
			{
				return ERR_SEND_WOULD_BLOCK;  //下次继续尝试发送
			}
		}

		//下次继续尝试发送
		if (nByteSent == 0)
		{
			return ERR_SEND_OK;
		}
	}
	return ERR_SEND_OK;
}

int CByteBuff::Recv(CSocket& socket)
{
	if (!socket.IsValid())
	{
		return ERR_RECV_NOSOCK;
	}

	SOCKET fd = socket.GetSocket();
	if (m_nReadIndex == m_nWriteIndex)
	{
		m_nReadIndex = 0;
		m_nWriteIndex = 0;
	}
	int nCanWriteSpace = CanWriteLen();
	int nDataLen = socket.CanReadLen();
	//剩余空间必须大于写入的数据长度，否则会头尾相连，无法区分缓冲区满了还是缓冲区是空的
	if (nDataLen >= nCanWriteSpace)
	{
		GrowBuff(nDataLen - nCanWriteSpace);
	}
	nCanWriteSpace = CanWriteLen();
	if (nDataLen >= nCanWriteSpace)
	{
		InitBuff();
		return ERR_RECV_NOBUFF;
	}
	int nCanRecvLen = MIN(nCanWriteSpace, m_nCapacity - m_nWriteIndex);
	int nByteRecved = 0;
	BYTE* pTempSrc = m_aData + m_nWriteIndex;
	int nReadLen = 0;
	do
	{
		nReadLen = recv(fd, (char*)pTempSrc, nCanRecvLen, 0);
		if (nReadLen > 0)
		{
			nByteRecved += nReadLen;
			m_nWriteIndex = (m_nWriteIndex + nReadLen) % m_nCapacity;
			pTempSrc += nReadLen;
			//尾部没有空间了，剩下的放到数组头部接收
			if (nByteRecved >= nCanRecvLen)
			{
				break;
			}
		}
		else if (nReadLen == 0)
		{
			return ERR_RECV_REMOTE_CLOSED;
		}
		else
		{
			if (errno != EAGAIN)
			{
				return ERR_RECV_SOCKET_ERROR;
			}
			else
			{
				return ERR_RECV_WOULD_BLOCK;
			}
		}
	} while (nReadLen > 0);

	nDataLen = socket.CanReadLen();
	//没有数据可读了
	if (nDataLen <= 0)
	{
		return ERR_RECV_OK;
	}

	//这里还有数据没有接收完，说明缓冲区的空闲区间在两头，上面写到了缓冲区的尾部，现在从缓冲区头部继续读取
	if (m_nWriteIndex != 0)
	{
		InitBuff();
		return ERR_RECV_UNKNOW_ERROR;
	}

	nByteRecved = 0;
	nCanWriteSpace = CanWriteLen();
	nCanRecvLen = MIN(nCanWriteSpace, m_nCapacity - m_nWriteIndex);
	pTempSrc = m_aData;
	do
	{
		nReadLen = recv(fd, (char*)pTempSrc, nCanRecvLen, 0);
		if (nReadLen > 0)
		{
			nByteRecved += nReadLen;
			m_nWriteIndex = (m_nWriteIndex + nReadLen) % m_nCapacity;
			pTempSrc += nReadLen;
			//没有空间用了
			if (nByteRecved >= nCanRecvLen)
			{
				InitBuff();
				return ERR_RECV_NOBUFF;
			}
		}
		else if (nReadLen == 0)
		{
			return ERR_RECV_REMOTE_CLOSED;
		}
		else
		{
			if (errno != EAGAIN)
			{
				return ERR_RECV_SOCKET_ERROR;
			}
			else
			{
				return ERR_RECV_WOULD_BLOCK;
			}
		}
	} while (nReadLen > 0);
	return ERR_RECV_OK;
}

//检查是否可缩小缓冲区
bool CByteBuff::ShrinkBuff(time_t mstimestamp)
{
	if (m_nMinSize == m_nMaxSize)
	{
		return false;
	}
	if (!m_ResizeTimer.IsBeginTimer())
	{
		m_ResizeTimer.BeginTimer(mstimestamp, TCP_CHECK_BUFF_RESIZE * 1000);
	}

	//统计利用率
	float fUseRate = CaclUseRate();
	if (m_fBuffUseRate < fUseRate)
	{
		m_fBuffUseRate = fUseRate;
	}

	//一定时间内利用率低于50%
	if (m_ResizeTimer.IsTimeout(mstimestamp))
	{
		if (m_fBuffUseRate < 0.5f)
		{
			int nCanReadLen = CanReadLen();
			int nNewCap = m_nCapacity * (2.0F / 3.0F);  //大小缩为2/3
			nNewCap = MAX(nCanReadLen, nNewCap);
			BYTE* pNewData = new BYTE[nNewCap];
			if (nCanReadLen > 0)
			{
				ReadBytes(pNewData, nCanReadLen, true);
			}
			m_nReadIndex = 0;
			m_nWriteIndex = nCanReadLen;
			DELETE_ARR(m_aData);
			m_aData = pNewData;
			m_nCapacity = nNewCap;
			m_fBuffUseRate = CaclUseRate();
			return true;
		}
	}
	return false;
}

//扩大缓冲区
void CByteBuff::GrowBuff(int newsize)
{
	int nNewCapacity = m_nCapacity >> 2;
	nNewCapacity = MAX(MAX(m_nCapacity + newsize + 1, nNewCapacity),m_nMaxSize);
	BYTE* pNewData = new BYTE[nNewCapacity];
	memcpy(pNewData, m_aData, m_nCapacity);
	DELETE_ARR(m_aData);
	m_aData = pNewData;
	m_nCapacity = nNewCapacity;
}


void CByteBuff::InitBuff()
{
	DELETE_ARR(m_aData);
	m_nCapacity = m_nMinSize;
	m_aData = new BYTE[m_nCapacity];
	m_nReadIndex = 0;
	m_nWriteIndex = 0;
	m_fBuffUseRate = 0.0f;
}

