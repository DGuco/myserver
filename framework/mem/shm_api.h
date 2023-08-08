//
//  sharemem.h
//  调用系统共享内存接口创建共享内存
//  Created by DGuco on 17/03/23.
//  Copyright © 2016年 DGuco. All rights reserved.
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
	// Qualifier: 生成共享内存块唯一id
	// Parameter: const char * pFile
	// Parameter: int vId
	//************************************
	//sm_key MakeKey(const char* pFile, int vId);
	//************************************
	// Method:    CreateShareMem
	// FullName:  ShareMemAPI::CreateShareMem
	// Access:    public 
	// Returns:   SMKey
	// Qualifier: 创建共享内存块
	// Parameter: SMKey iKey
	// Parameter: int vSize
	//************************************
	sm_handler CreateShareMem(sm_key iKey, size_t vSize);
	//************************************
	// Method:    OpenShareMem
	// FullName:  ShareMemAPI::OpenShareMem
	// Access:    public 
	// Returns:   SMKey
	// Qualifier: /打开共享内存
	// Parameter: SMKey iKey
	// Parameter: int vSize
	//************************************
	sm_handler OpenShareMem(sm_key iKey, size_t vSize);
	//************************************
	// Method:    AttachShareMem
	// FullName:  ShareMemAPI::AttachShareMem
	// Access:    public 
	// Returns:   BYTE*
	// Qualifier: attach 到共享内存块中
	// Parameter: SMKey iKey
	//************************************
	BYTE* AttachShareMem(sm_handler iKey);
	//************************************
	// Method:    DetachShareMem
	// FullName:  ShareMemAPI::DetachShareMem
	// Access:    public 
	// Returns:   bool
	// Qualifier: Detach 共享内存块
	// Parameter: BYTE * pAddr
	//************************************
	bool DetachShareMem(BYTE* pAddr);
	//************************************
	// Method:    DestroyShareMem
	// FullName:  ShareMemAPI::DestroyShareMem
	// Access:    public 
	// Returns:   int
	// Qualifier: 销毁共享内存块
	// Parameter: SMKey iKey
	//************************************
	bool DestroyShareMem(sm_handler iKey);
};


#endif // __SHARE_MEM_H__

