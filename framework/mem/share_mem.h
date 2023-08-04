//
//  sharemem.h
//  调用系统共享内存接口创建共享内存
//  Created by DGuco on 17/03/23.
//  Copyright © 2016年 DGuco. All rights reserved.
//
#ifndef __SHARE_MEM_H__
#define __SHARE_MEM_H__

#include "platform_def.h"

#ifdef __LINUX__
typedef INT SMKey;
#else
typedef void* SMKey;
#endif

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
	SMKey MakeKey(const char* pFile, int vId);
	//************************************
	// Method:    CreateShareMem
	// FullName:  ShareMemAPI::CreateShareMem
	// Access:    public 
	// Returns:   SMKey
	// Qualifier: 创建共享内存块
	// Parameter: SMKey iKey
	// Parameter: int vSize
	//************************************
	SMKey CreateShareMem(SMKey iKey, int vSize);
	//************************************
	// Method:    OpenShareMem
	// FullName:  ShareMemAPI::OpenShareMem
	// Access:    public 
	// Returns:   SMKey
	// Qualifier: /打开共享内存
	// Parameter: SMKey iKey
	// Parameter: int vSize
	//************************************
	SMKey OpenShareMem(SMKey iKey, int vSize);
	//************************************
	// Method:    AttachShareMem
	// FullName:  ShareMemAPI::AttachShareMem
	// Access:    public 
	// Returns:   BYTE*
	// Qualifier: attach 到共享内存块中
	// Parameter: SMKey iKey
	//************************************
	BYTE* AttachShareMem(SMKey iKey);
	//************************************
	// Method:    DetachShareMem
	// FullName:  ShareMemAPI::DetachShareMem
	// Access:    public 
	// Returns:   int
	// Qualifier: Detach 共享内存块
	// Parameter: BYTE * pAddr
	//************************************
	int DetachShareMem(BYTE* pAddr);
	//************************************
	// Method:    DestroyShareMem
	// FullName:  ShareMemAPI::DestroyShareMem
	// Access:    public 
	// Returns:   int
	// Qualifier: 销毁共享内存块
	// Parameter: SMKey iKey
	//************************************
	int DestroyShareMem(SMKey iKey);
};


#endif // __SHARE_MEM_H__

