//
// byte_buff.h
// Created by DGuco on 18/04/13.
// Copyright © 2018年 DGuco. All rights reserved.
// 网络字节流管理类
//

#ifndef SERVER_CBYTEBUFF_H
#define SERVER_CBYTEBUFF_H

#include <base.h>
#include <string>

using namespace std;


/**
 * 最大长度应该减去预留部分长度，保证首尾不会相接,
 * 以此区分数据分布在内存区的两侧，和没有数据的情况
 */
#define BUFF_EXTRA_SIZE (8)

class CByteBuff
{
public:
	//构造函数
	CByteBuff();
	CByteBuff(msize_t tmpCap);
	//拷贝构造函数
	CByteBuff(const CByteBuff &byteBuff);
	//移动构造函数
	CByteBuff(CByteBuff &&byteBuff);
	//赋值操作符
	CByteBuff &operator=(CByteBuff &byteBuff);
	//移动赋值操作符
	CByteBuff &operator=(CByteBuff &&byteBuff);
	~CByteBuff();
	void Clear();
	//大小端转换
	BYTE* Flip(BYTE*netStr, size_t len);
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
	int  ReadBytes(BYTE* pOutCode, msize_t tmLen, bool ispeek = false);
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
	int  WriteBytes(BYTE* pInCode, msize_t tmLen);
	unsigned int GetReadIndex() const;
	unsigned int GetWriteIndex() const;
	unsigned int GetCapaticy() const;
	void ResetReadIndex();
	void ResetWriteIndex();
	void WriteLen(msize_t len);
	void ReadLen(msize_t len);
	void SetReadIndex(msize_t uiReadIndex);
	void SetWriteIndex(msize_t uiWriteIndex);
	//获取可读数据长度
	msize_t CanReadLen() const;
	//获取可写数据长度
	msize_t CanWriteLen() const;
	//
	BYTE* GetData() const;
	//获取可读数据
	BYTE* CanReadData() const;
	//获取可写数据空间
	BYTE* CanWriteData() const;
public:
	//判断是否是小端
	static bool IsLittleEndian();
	static void Reverse(BYTE*str, size_t len);
private:
	template<class T,int len_ = sizeof(T)> //只读取基本类型
	T ReadT(bool ispeek);
	/**
	 *
	 * @tparam T
	 * @param t
	 * @param pos 	相对writeindex 的偏移
	 */
	template<class T, int len_ = sizeof(T)> //只写基本类型
	void WriteT(T t, int offset = 0);
	void Copy(const CByteBuff *srcBuff);
private:
	static bool m_bIsLittleEndian;
private:
	msize_t m_nReadIndex;
	msize_t m_nWriteIndex;
	msize_t m_nCapacity;
	BYTE*	m_aData;
};


#endif //SERVER_CBYTEBUFF_H
