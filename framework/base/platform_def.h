/*****************************************************************
* FileName:platform_def.h
* Summary :平台相关定义
* Date	  :2023-7-28
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#ifdef __LINUX__
//内存屏障
#define __MEM_BARRIER \
            __asm__ __volatile__("mfence":::"memory")
		//内存读屏障
#define __READ_BARRIER__ \
            __asm__ __volatile__("lfence":::"memory")
		//内存写屏障
#define __WRITE_BARRIER__ \
            __asm__ __volatile__("sfence":::"memory")
#else
//内存屏障
#define __MEM_BARRIER \
           ()
		//内存读屏障
#define __READ_BARRIER__ \
           ()
		//内存写屏障
#define __WRITE_BARRIER__ \
           ()
#endif

#endif //__PLATFORM_DEF_H__



