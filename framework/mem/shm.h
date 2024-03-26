//
//  shm.h
//  �����ڴ�������
//  Created by DGuco on 17/03/23.
//  Copyright ? 2016�� DGuco. All rights reserved.
//

#ifndef _SHM_H_
#define _SHM_H_
#include "platform_def.h"
#include "base.h"

#define YEARSEC 31536000

#define MAX_SOCKET_NUM	     (65536)

struct SSmHead
{
	//�����ڴ�key
	unsigned int	m_nShmKey;
	//�����ڴ��С
	size_t			m_nShmSize;
	//��ʼ��ַ
	BYTE*			m_pSegment;
	sm_handler      m_Handler;
};

class CSharedMem
{
public:
    //���캯��
	CSharedMem();
public:
    //��������
    ~CSharedMem();
//     //new ���������ؽ�������嵽�����ڴ�����ַ
// 	void* operator new( size_t nSize);
//     //delete ����������
// 	void  operator delete(void* pMem);
	//��ʼ��
	bool Init(eShmModule module,sm_key nSmKey, size_t nSize);
	//��ȡ���󴴽�����
	eShmModule GetInitMode();
    //���ö��󴴽�����
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
	//�����ڴ����Ϣ
	SSmHead*				m_pHead;
	//��ǰ���ÿ����ڴ�ȥ����ʼ��ַ
	BYTE*					m_pCurrentSegMent;
	//�����ڴ�Ĵ�С
	size_t					m_nSize;
    //�����ʼ������
	eShmModule				m_InitMode;
};


#endif
