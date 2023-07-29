/*****************************************************************
* FileName:platform_def.h
* Summary :
* Date	  :2023-7-28
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#ifdef __LINUX__
    //�ڴ�����
    #define __MEM_BARRIER \
        __asm__ __volatile__("mfence":::"memory")
    //�ڴ������
    #define __READ_BARRIER__ \
        __asm__ __volatile__("lfence":::"memory")
    //�ڴ�д����
    #define __WRITE_BARRIER__ \
        __asm__ __volatile__("sfence":::"memory")
#else
#include <windows.h>
    //�ڴ�����
    #define __MEM_BARRIER MemoryFence 
    //�ڴ������
    #define __READ_BARRIER__ LoadFence
    //�ڴ�д����
    #define __WRITE_BARRIER__ StoreFence
#endif

#endif //__PLATFORM_DEF_H__
