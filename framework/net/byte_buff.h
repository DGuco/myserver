//
// byte_buff.h
// Created by DGuco on 18/04/13.
// Copyright ? 2018�� DGuco. All rights reserved.
// �����ֽ���������
//

#ifndef SERVER_CBYTEBUFF_H
#define SERVER_CBYTEBUFF_H

#include <base.h>
#include <string>
#include "time_helper.h"
#include "socket.h"
using namespace std;

/**
 * ��󳤶�Ӧ�ü�ȥԤ�����ֳ��ȣ���֤��β�������,
 * �Դ��������ݷֲ����ڴ��������࣬��û�����ݵ����
 */
#define BUFF_EXTRA_SIZE (8)
 //���¶������/���ʹ�������
enum eRecvErrs
{
	ERR_RECV_WOULD_BLOCK		= 1,
	ERR_RECV_OK					=  0,
	ERR_RECV_NOT_READY			= -1,
	ERR_RECV_NOSOCK				= -2,
	ERR_RECV_NOBUFF				= -3,
	ERR_RECV_REMOTE_CLOSED		= -4,
	ERR_RECV_SOCKET_ERROR		= -5,
	ERR_RECV_PARSE_ERROR		= -6,
};

enum eSendErrs
{
	ERR_SEND_WOULD_BLOCK	= 1,
	ERR_SEND_OK				= 0,
	ERR_SEND_NOT_READY		= -1,
	ERR_SEND_NOSOCK			= -2,
	ERR_SEND_NOBUFF			= -3,
	ERR_SEND_NODATA			= -4,
	ERR_SEND_SOCKET_ERROR	= -5,
};

class CByteBuff
{
public:
	//���캯��
	CByteBuff();
	CByteBuff(int minsize,int maxsize = 0);
	//�������캯��
	CByteBuff(const CByteBuff &byteBuff);
	//��ֵ������
	CByteBuff &operator=(CByteBuff &byteBuff);
	//��������
	~CByteBuff();
	void Clear();
	short ReadShort(bool ispeek = false);
	int ReadInt(bool ispeek = false);
	long ReadLong(bool ispeek = false);
	long long ReadLongLong(bool ispeek = false);
	unsigned short ReadUnShort(bool ispeek = false);
	unsigned int ReadUnInt(bool ispeek = false);
	unsigned long ReadUnLong(bool ispeek = false);
	unsigned long long ReadUnLongLong(bool ispeek = false);
	float ReadFloat(bool ispeek = false);
	double ReadDouble(bool ispeek = false);
	int  ReadBytes(BYTE* pOutCode, int tmLen, bool ispeek = false);
	void WriteShort(short value, int offset = 0);
	void WriteInt(int value, int offset = 0);
	void WriteLong(long value, int offset = 0);
	void WriteLongLong(long long value, int offset = 0);
	void WriteUnShort(unsigned short value, int offset = 0);
	void WriteUnInt(unsigned int value, int offset = 0);
	void WriteUnLong(unsigned long value, int offset = 0);
	void WriteUnLongLong(unsigned long long value, int offset = 0);
	void WriteFloat(float value, int offset = 0);
	void WriteDouble(double value, int offset = 0);
	int  WriteBytes(BYTE* pInCode, int tmLen);
	int GetReadIndex() const;
	int GetWriteIndex() const;
	int GetCapaticy() const;
	void ResetReadIndex();
	void ResetWriteIndex();
	void WriteLen(int len);
	void ReadLen(int len);
	void SetReadIndex(int uiReadIndex);
	void SetWriteIndex(int uiWriteIndex);
	//��ȡ�ɶ����ݳ���
	int CanReadLen() const;
	//��ȡ��д���ݳ���
	int CanWriteLen() const;
	//��ȡ����ָ��
	BYTE* GetData() const;
	//���ͻ��������ݵ�tcp
	int Send(CSocket& socket);
	//��������
	int Recv(CSocket& socket);
	//����Ƿ����С������
	bool CheckResizeBuff(time_t mstimestamp);
	//ֻ��ȡ��������
	template<class T, int len_ = sizeof(T)>
	T ReadT(bool ispeek = false);
public:
	//�ж��Ƿ���С��
	static bool IsLittleEndian();
	static void Reverse(BYTE*str, size_t len);
private:
	//��С��ת��
	BYTE* Flip(BYTE* netStr, size_t len);
	/**
	 *
	 * @tparam T
	 * @param t
	 * @param pos 	���writeindex ��ƫ��
	 */
	template<class T, int len_ = sizeof(T)> //ֻд��������
	void WriteT(T t, int offset = 0);
	//��Ҫ������
	void Copy(const CByteBuff& srcBuff);
	//����������
	float CaclUseRate();
private:
	static bool m_bIsLittleEndian;
private:
	int			m_nReadIndex;
	int			m_nWriteIndex;
	int			m_nCapacity;
	int			m_nMinSize;
	int			m_nMaxSize;
	CMyTimer	m_ResizeTimer;   //����������timer
	float		m_fBuffUseRate;  //������������
	BYTE*		m_aData;
};


#endif //SERVER_CBYTEBUFF_H
