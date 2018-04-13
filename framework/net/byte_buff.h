//
// byte_buff.h
// Created by DGuco on 18/04/13.
// Copyright © 2018年 DGuco. All rights reserved.
// 字节管理类
//

#ifndef SERVER_CBYTEBUFF_H
#define SERVER_CBYTEBUFF_H


#include <base.h>

class CByteBuff
{
public:
	CByteBuff();

public:
	//判断是否是小端
	static bool IsLittleEndian();
private:
	static bool m_bIsLittleEndian;
private:
	unsigned int m_iReadIndex;
	unsigned int m_iWriteIndex;
	unsigned int m_iLen;
	char m_acData[MAX_PACKAGE_LEN];
};


#endif //SERVER_CBYTEBUFF_H
