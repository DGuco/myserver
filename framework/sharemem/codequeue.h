//
//  codequeue.h
//  共享内存操作接口类
//  Created by DGuco on 17/03/23.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _CODE_QUEUE_H_
#define _CODE_QUEUE_H_

#include "shm.h"

#define QUEUERESERVELENGTH		8	//预留部分的长度

class CCodeQueue
{
public:
	//构造函数
	CCodeQueue();
	//构造函数
	CCodeQueue( int nTotalSize, int iLockIdx = -1 );
	//析构函数
	~CCodeQueue();
	//new操作符重载将类对象定义到共享内存区地址
	void* operator new( size_t nSize );
	//delete操作符重载
	void  operator delete( void *pBase );
	//初始化
	int Initialize( int nTotalSize );
	//恢复
	int Resume( int nTotalSize );
	//向进程间共享内存管道插入数据
	int AppendOneCode(const BYTE *pInCode, int sInLength);
	//从进程间共享内存管道获取数据
	int GetHeadCode(BYTE *pOutCode, int *psOutLength);
	//从进程间共享内存管道获取数据
	int PeekHeadCode(BYTE *pOutCode, int *psOutLength);
	//从进程间共享内存管道删除数据
	int DeleteHeadCode();
	//从进程间共享内存管道获取数据
	int GetOneCode(int iCodeOffset, int nCodeLength, BYTE *pOutCode, int *psOutLength);
	//共享内存管道是否空闲
	int IsQueueEmpty();
	//向文件中备份数据
	int DumpToFile(const char *szFileName);
	//从文件中加载数据
	int LoadFromFile(const char *szFileName);
	//清除进程间共享内存管道数据
	int CleanQueue();

	//codequeue大小
	static size_t CountQueueSize(int iBufSize);

	static CSharedMem *pCurrentShm;


	void GetCriticalData(int& iBegin, int& iEnd, int& iLeft);

protected:
	
private:
	int IsQueueFull();
	int SetFullFlag( int iFullFlag );

	int GetCriticalData(int *piBeginIdx, int *piEndIdx);
	int SetCriticalData(int iBeginIdx, int iEndIdx);

	struct _tagHead
	{
		int m_iSize;                //共享内存管道大小
		int m_iCodeBufOffSet;       //共享内存管道地址在CCodeQueue对象中的偏移地址
		int m_iReadIndex;           //读数据索引
		int m_iWriteIndex;          //写数据索引
		int m_iLockIdx;             //数据锁id
	} m_stQueueHead;

    //共享内存管道起始地址
	BYTE* m_pbyCodeBuffer;
};

#endif
