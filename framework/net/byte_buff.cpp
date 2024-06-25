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
	BYTE* pTempDst = pOutCode;  // ���ý��� Code �ĵ�ַ
	int nReadLen = MIN(usOutLength, m_nCapacity - m_nReadIndex);
	memcpy((void*)pTempDst, (const void*)(pTempSrc + m_nReadIndex), nReadLen);
	int tmpLast = usOutLength - nReadLen;
	//�������������ȥͷ��������ȡ
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
	//ʣ��ռ�������д������ݳ��ȣ������ͷβ�������޷����ֻ��������˻��ǻ������ǿյ�
	if (tmLen >= nCanWriteSpace)
	{
		if (m_nCapacity < m_nMaxSize)
		{
			GrowBuff(tmLen - nCanWriteSpace);
			m_fBuffUseRate = CaclUseRate();
			nCanWriteSpace = CanWriteLen();
			if (tmLen >= nCanWriteSpace)//ʣ��ռ䲻��
			{
				InitBuff();
				return -1;
			}
		}
		else //ʣ��ռ䲻��
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
	//�����ʣ�࣬˵�����в������ڴ����ͷ����ͷ��������
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
	//��Ϊ��֪�����ͷ��Ǵ�˻���С�ˣ�����Ĭ�Ϸ��ͷ�����ת���ɴ�˷���(�����ֽ���Ĭ���Դ����ʽ����)��
	//���������С�ˣ����յ����ݺ�Ѵ���ֽ���ת����С��Ȼ����ʹ��
	/* plus
	  �ַ����ַ��� �������ַ�Ϊ��λ�ģ����Զ�д����ʱ�����д�С�����⣻
	  ��ֵ(short / int / float / double / ......)���ж���ַ���ɣ��ڶ�дʱ���д�С�ˣ�
	  �ַ����������Ϊ���ֽڵ��ַ����飬�ַ�֮��û��ֱ�ӹ������������ֽ������⣻
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
	//��Ϊ��֪�����շ��Ǵ�˻���С�ˣ�����Ĭ�Ϸ��ͷ�����ת���ɴ�˷���(�����ֽ���Ĭ���Դ����ʽ����)��
	//���������С�ˣ�����ǰ��С���ڴ���ת��Ϊ��������������ٷ���
	/* plus
	  �ַ����ַ��� �������ַ�Ϊ��λ�ģ����Զ�д����ʱ�����д�С�����⣻
	  ��ֵ(short / int / float / double / ......)���ж���ַ���ɣ��ڶ�дʱ���д�С�ˣ�
	  �ַ����������Ϊ���ֽڵ��ַ����飬�ַ�֮��û��ֱ�ӹ������������ֽ������⣻
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
//����������
float CByteBuff::CaclUseRate()
{
	//ͳ��������
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
		//��ȡ���ݴ�С
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
	//���ʣ��ռ��С
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
	//û������
	if (nCanReadSpace <= 0)
	{
		return ERR_SEND_OK;
	}
	
	SOCKET fd = socket.GetSocket();
	//���ݿ������м�Ҳ���������ߣ��ȷ���β��������
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

		//���ͳ���
		if (nByteSent < 0)
		{
			if (errno != OPT_WOULD_BLOCK)
			{
				return ERR_SEND_SOCKET_ERROR;
			}
			else
			{
				return ERR_SEND_WOULD_BLOCK;  //�´μ������Է���
			}
		}

		//�´μ������Է���
		if (nByteSent == 0)
		{
			return ERR_SEND_OK;
		}
	}

	nCanReadSpace = CanReadLen();
	//��������
	if (nCanReadSpace <= 0)
	{
		return ERR_SEND_OK;
	}

	/*����������м䣬��һ��û��ȫ�����ͳ�ȥ��˵�����������ˣ��޷��������ͣ�ֱ�ӷ��صȴ��´η���
	����Ӧ�ò����ߵ�����޷���������send����ֵ�᷵��0���߷���OPT_WOULD_BLOCK�������ֱ�ӷ���
	�����ߵ�����,������������ߣ�β��������ɺ�m_nReadIndex�϶�����0�����������0��˵�������
	β�����ݷ����߼���û�з����꣬���������ˣ����ټ������ͣ��´μ������Է���
	*/
	if (m_nReadIndex != 0)
	{
		return ERR_SEND_OK;
	}

	//�������ͻ�����ͷ��������
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

		//���ͳ���
		if (nByteSent < 0)
		{
			if (errno != OPT_WOULD_BLOCK)
			{
				return ERR_SEND_SOCKET_ERROR;
			}
			else
			{
				return ERR_SEND_WOULD_BLOCK;  //�´μ������Է���
			}
		}

		//�´μ������Է���
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
	//ʣ��ռ�������д������ݳ��ȣ������ͷβ�������޷����ֻ��������˻��ǻ������ǿյ�
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
			//β��û�пռ��ˣ�ʣ�µķŵ�����ͷ������
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
	//û�����ݿɶ���
	if (nDataLen <= 0)
	{
		return ERR_RECV_OK;
	}

	//���ﻹ������û�н����꣬˵���������Ŀ�����������ͷ������д���˻�������β�������ڴӻ�����ͷ��������ȡ
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
			//û�пռ�����
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

//����Ƿ����С������
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

	//ͳ��������
	float fUseRate = CaclUseRate();
	if (m_fBuffUseRate < fUseRate)
	{
		m_fBuffUseRate = fUseRate;
	}

	//һ��ʱ���������ʵ���50%
	if (m_ResizeTimer.IsTimeout(mstimestamp))
	{
		if (m_fBuffUseRate < 0.5f)
		{
			int nCanReadLen = CanReadLen();
			int nNewCap = m_nCapacity * (2.0F / 3.0F);  //��С��Ϊ2/3
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

//���󻺳���
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

