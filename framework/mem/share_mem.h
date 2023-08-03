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
	//创建共享内存块
	BYTE* CreateShareMem(SMKey iKey, int vSize);

	//销毁共享内存块
	int DestroyShareMem(SMKey iKey);

};


#endif // __SHARE_MEM_H__

