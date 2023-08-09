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

class CByteBuff
{
public:
	//构造函数
	CByteBuff();
	CByteBuff(unsigned int tmpCap);
	CByteBuff(BYTE*data, unsigned int tmpCap);
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
	short ReadShort();
	int ReadInt();
	long ReadLong();
	long long ReadLongLong();
	unsigned short ReadUnShort();
	unsigned int ReadUnInt();
	unsigned long ReadUnLong();
	unsigned long long ReadUnLongLong();
	float ReadFloat();
	double ReadDouble();
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

	unsigned int GetReadIndex() const;
	unsigned int GetWriteIndex() const;
	unsigned int GetCapaticy() const;
	void ResetReadIndex();
	void ResetWriteIndex();
	void WriteLen(unsigned int len);
	void ReadLen(unsigned int len);
	void SetReadIndex(unsigned int uiReadIndex);
	void SetWriteIndex(unsigned int uiWriteIndex);
	unsigned int WriteableDataLen() const;
	unsigned int ReadableDataLen() const;
	BYTE* GetData() const;
	//获取可读数据
	BYTE* CanReadData() const;
	//获取可写数据空间
	BYTE* CanWriteData() const;
	void  ReadBytes(BYTE*data, unsigned int len);
	void  WriteBytes(BYTE*data, unsigned int len);
public:
	//判断是否是小端
	static bool IsLittleEndian();
	static void Reverse(BYTE*str, size_t len);
private:
	template<class T> //只读取基本类型
	T ReadT();
	/**
	 *
	 * @tparam T
	 * @param t
	 * @param pos 	相对writeindex 的偏移
	 */
	template<class T> //只写基本类型
	void WriteT(T t, int offset = 0);
	void Copy(const CByteBuff *srcBuff);
private:
	static bool m_bIsLittleEndian;
private:
	unsigned int m_uiReadIndex;
	unsigned int m_uiWriteIndex;
	unsigned int m_uiLen;
	unsigned int m_uiCapacity;
	BYTE*		 m_acData;
	string		 a;
};


#endif //SERVER_CBYTEBUFF_H
