/*****************************************************************
* FileName:platform_def.h
* Summary :
* Date	  :2023-7-28
* Author  :DGuco(1139140909@qq.com)

******************************************************************/
#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#ifdef __LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#else
#include <WinSock2.h>
#include <Windows.h>
#include <WinBase.h>
#endif
#include <string>
using namespace std;

#ifdef __LINUX__
    //
    #define __MEM_BARRIER \
        __asm__ __volatile__("mfence":::"memory")
    #define __READ_BARRIER__ \
        __asm__ __volatile__("lfence":::"memory")
    #define __WRITE_BARRIER__ \
        __asm__ __volatile__("sfence":::"memory")

    #define OPT_WOULD_BLOCK   (EAGAIN)
    #define SOCKET_CONNECTING  (EINPROGRESS)
    #define SLEEP(miseconds) sleep(miseconds)
    typedef int sm_handler;
    typedef int sm_key;
	typedef int	SOCKET;
#else
    std::string GetErrorMessage(int errorCode)
    {
	    LPSTR errorMessage = nullptr;
	    DWORD result = FormatMessageA(
		    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		    NULL, 
            errorCode, 
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		    (LPSTR)&errorMessage,
            0, 
            NULL);
	    if (result == 0)
        {
		    return "Failed to get error message";
	    }
	    std::string errorMessageStr(errorMessage);
	    LocalFree(errorMessage);
	    return errorMessageStr;
    }

    #define errno (WSAGetLastError())
    #define strerror(code) (GetErrorMessage(code))
    #define __MEM_BARRIER MemoryFence 
    #define __READ_BARRIER__ LoadFence
    #define __WRITE_BARRIER__ StoreFence

    #define OPT_WOULD_BLOCK   (WSAEWOULDBLOCK)
    #define SOCKET_CONNECTING  (WSAEWOULDBLOCK)
    #define SLEEP(miseconds) Sleep(miseconds)

    typedef int sm_key;
    typedef void* sm_handler;
#endif

#endif //__PLATFORM_DEF_H__
