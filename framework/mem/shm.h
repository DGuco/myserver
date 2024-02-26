//
//  shm.h
//  共享内存块管理类
//  Created by DGuco on 17/03/23.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _SHM_H_
#define _SHM_H_
#include "platform_def.h"
#include "base.h"

#define YEARSEC 31536000

#define PIPE_SIZE            (0x1000000)    // 共享内存管道大小 16M
#define C2S_SHM_KEY			 (20000000)
#define S2C_SHM_KEY			 (20000001)
#define MAX_SOCKET_NUM	     (65536)

struct SSmHead
{
	//共享内存key
	unsigned int	m_nShmKey;
	//共享内存大小
	size_t			m_nShmSize;
	//起始地址
	BYTE*			m_pSegment;
};

class CSharedMem
{
public:
    //构造函数
	CSharedMem();
public:
    //析构函数
    ~CSharedMem();
//     //new 操作符重载将类对象定义到共享内存区地址
// 	void* operator new( size_t nSize);
//     //delete 操作符重载
// 	void  operator delete(void* pMem);
	//初始化
	bool Init(EIMode module,sm_key nSmKey, size_t nSize);
	//获取对象创建类型
	EIMode GetInitMode();
    //设置对象创建类型
	bool CreateSegment(sm_key nSmKey,size_t nSize);
	//attach
	bool AttachSegment(sm_key nSmKey, size_t nSize);
	//Detach
	bool DetachSegment();
	//Close
	bool CloseSegment();
	//
	BYTE* GetSegment();
private:
	//共享内存块信息
	SSmHead*				m_pHead;
	//当前可用空闲内存去的起始地址
	BYTE*					m_pCurrentSegMent;
	//可用内存的大小
	size_t					m_nSize;
    //对象初始化类型
	EIMode					m_InitMode;
	//共享内存句柄
	sm_handler				m_Handler;
};


#endif
