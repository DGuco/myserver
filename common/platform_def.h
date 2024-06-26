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
	#define INVALID_SM_HADLER (-1)

    typedef int sm_handler;
    typedef int sm_key;
	typedef int	SOCKET;
	typedef pthread_t	TID;
#else
    std::string GetErrorMessage(int errorCode);

    #define strerror(code) (GetErrorMessage(code))
    #define __MEM_BARRIER MemoryFence 
    #define __READ_BARRIER__ LoadFence
    #define __WRITE_BARRIER__ StoreFence

    #define OPT_WOULD_BLOCK   (WSAEWOULDBLOCK)
    #define SOCKET_CONNECTING  (WSAEWOULDBLOCK)
    #define SLEEP(miseconds) Sleep(miseconds)

    typedef int sm_key;
    typedef void* sm_handler;
	#define INVALID_SM_HADLER (NULL)
	typedef DWORD		TID;

#endif

#define CACHE_LINE_SIZE 64
//�޸��ֶ�����򣬱���false sharing
#define CACHE_LINE_ALIGN  __attribute__((aligned(CACHE_LINE_SIZE)))

TID  MyGetCurrentThreadID();
// �ܵ���ʶ��
enum enLockIdx
{
	IDX_PIPELOCK_C2S = 0,
	IDX_PIPELOCK_S2C = 1,
	IDX_PIPELOCK_A2C = 2,
	IDX_PIPELOCK_S2L = 3,
};
enum eQueueErrorCode
{
	QUEUE_OK = 0,     // param error
	QUEUE_PARAM_ERROR = -1,     // param error
	QUEUE_NO_SPACE = -2,        // message queue has no space
	QUEUE_NO_MESSAGE = -3,      // message queue has no message
	QUEUE_DATA_SEQUENCE_ERROR = -4,// message queue the message sequence error
};

enum eQueueModel
{
	ONE_READ_ONE_WRITE,   //һ�����̶���Ϣһ������д��Ϣ
	ONE_READ_MUL_WRITE,   //һ�����̶���Ϣ�������д��Ϣ
	MUL_READ_ONE_WRITE,   //������̶���Ϣһ������д��Ϣ
	MUL_READ_MUL_WRITE,   //������̶���Ϣ�������д��Ϣ
};

enum eShmModule
{
	SHM_INVALID = -1,
	SHM_INIT,     //��һ�����빲���ڴ棬��ʼ��
	SHM_RESUME,   //�����ڴ��Ѵ��ڣ��ָ�����ӳ�乲���ڴ�����
};
#endif //__PLATFORM_DEF_H__
