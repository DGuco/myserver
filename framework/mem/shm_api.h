//
//  sharemem.h
//  ����ϵͳ�����ڴ�ӿڴ��������ڴ�
//  Created by DGuco on 17/03/23.
//  Copyright ? 2016�� DGuco. All rights reserved.
//
#ifndef __SHARE_MEM_H__
#define __SHARE_MEM_H__

#include "platform_def.h"

namespace ShareMemAPI
{
	//************************************
	// Method:    MakeKey
	// FullName:  ShareMemAPI::MakeKey
	// Access:    public 
	// Returns:   SMKey
	// Qualifier: ���ɹ����ڴ��Ψһid
	// Parameter: const char * pFile
	// Parameter: int vId
	//************************************
	//sm_key MakeKey(const char* pFile, int vId);
	//************************************
	// Method:    CreateShareMem
	// FullName:  ShareMemAPI::CreateShareMem
	// Access:    public 
	// Returns:   SMKey
	// Qualifier: ���������ڴ��
	// Parameter: SMKey iKey
	// Parameter: int vSize
	//************************************
	sm_handler CreateShareMem(sm_key iKey, size_t vSize);
	//************************************
	// Method:    OpenShareMem
	// FullName:  ShareMemAPI::OpenShareMem
	// Access:    public 
	// Returns:   SMKey
	// Qualifier: /�򿪹����ڴ�
	// Parameter: SMKey iKey
	// Parameter: int vSize
	//************************************
	sm_handler OpenShareMem(sm_key iKey, size_t vSize);
	//************************************
	// Method:    AttachShareMem
	// FullName:  ShareMemAPI::AttachShareMem
	// Access:    public 
	// Returns:   BYTE*
	// Qualifier: attach �������ڴ����
	// Parameter: SMKey iKey
	//************************************
	BYTE* AttachShareMem(sm_handler iKey);
	//************************************
	// Method:    DetachShareMem
	// FullName:  ShareMemAPI::DetachShareMem
	// Access:    public 
	// Returns:   bool
	// Qualifier: Detach �����ڴ��
	// Parameter: BYTE * pAddr
	//************************************
	bool DetachShareMem(BYTE* pAddr);
	//************************************
	// Method:    DestroyShareMem
	// FullName:  ShareMemAPI::DestroyShareMem
	// Access:    public 
	// Returns:   int
	// Qualifier: ���ٹ����ڴ��
	// Parameter: SMKey iKey
	//************************************
	bool DestroyShareMem(sm_handler iKey);
};


#endif // __SHARE_MEM_H__

