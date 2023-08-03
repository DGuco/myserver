/*****************************************************************
* FileName:platform_def.h
* Summary :
* Date	  :2023-7-28
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#ifdef __LINUX__
    //
    #define __MEM_BARRIER \
        __asm__ __volatile__("mfence":::"memory")
    #define __READ_BARRIER__ \
        __asm__ __volatile__("lfence":::"memory")
    #define __WRITE_BARRIER__ \
        __asm__ __volatile__("sfence":::"memory")
#else
#include <windows.h>
    #define __MEM_BARRIER MemoryFence 
    #define __READ_BARRIER__ LoadFence
    #define __WRITE_BARRIER__ StoreFence

#endif

#endif //__PLATFORM_DEF_H__
