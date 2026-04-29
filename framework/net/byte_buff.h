//
// byte_buff.h
// Created by DGuco on 18/04/13.
// Copyright ? 2018年 DGuco. All rights reserved.
// 网络字节流管理类
//

#ifndef SERVER_CBYTEBUFF_H
#define SERVER_CBYTEBUFF_H

#include <base.h>
#include <string>
#include "time_helper.h"
#include "socket.h"
using namespace std;

class CByteBuff
{
public:
	//构造函数
	CByteBuff();
	CByteBuff(int minsize,int maxsize = 0);
	//拷贝构造函数
	CByteBuff(const CByteBuff &byteBuff);
	//赋值操作符
	CByteBuff &operator=(CByteBuff &byteBuff);
	//析构函数
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
	//获取可读数据长度
	int CanReadLen() const;
	//获取可写数据长度
	int CanWriteLen() const;
	//获取数据指针
	BYTE* GetData() const;
	//发送缓冲区数据到tcp
	int Send(CSocket& socket);
	//接收数据
	int Recv(CSocket& socket);
	//检查是否可缩小缓冲区
	bool ShrinkBuff(time_t mstimestamp);
	//扩大缓冲区
	void GrowBuff(int newsize);
	//
	void InitBuff();
public:
	//只读取基本类型
	template<class T,int len_ = sizeof(T)>
	T ReadT(bool ispeek = false)
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

		//T result = *(T *) tmpData;
		T result;
		std::memcpy(&result, tmpData, len_);
		return result;
	}
public:
	//判断是否是小端
	static bool IsLittleEndian();
	static void Reverse(BYTE*str, size_t len);
private:
	//大小端转换
	BYTE* Flip(BYTE* netStr, size_t len);
	/**
	 *
	 * @tparam T
	 * @param t
	 * @param pos 	相对writeindex 的偏移
	 */
	template<class T, int len_ = sizeof(T)> //只写基本类型
	void WriteT(T t, int offset = 0)
	{
		BYTE tmpData[len_];
		//*(T *) tmpData = t;
		std::memcpy(tmpData, &t, len_);
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
	//不要随便调用
	void Copy(const CByteBuff& srcBuff);
	//计算利用率
	float CaclUseRate();
private:
	static bool m_bIsLittleEndian;
private:
	int			m_nReadIndex;
	int			m_nWriteIndex;
	int			m_nCapacity;
	int			m_nMinSize;
	int			m_nMaxSize;
	CMyTimer	m_ResizeTimer;   //缓冲区回收timer
	float		m_fBuffUseRate;  //缓冲区利用率
	BYTE*		m_aData;
};


#endif //SERVER_CBYTEBUFF_H
