#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#ifdef __LINUX__
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#else
#endif
#include "base.h"
#include "platform_def.h"
#include "share_mem.h"
#include "log.h"
#include "t_array.h"

using namespace my_std;

SMKey ShareMemAPI::MakeKey(const char *pFile, int vId)
{
#ifdef __LINUX__
	return ftok(pFile, vId);
#else
#ifdef UNICODE
	TArray<wchar_t, 4096> wString;
	MultiByteToWideChar(CP_ACP, 0, pFile, -1, wString.GetData(), 4096);
	HANDLE hFile = CreateFile(wString,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
#else
	HANDLE hFile = CreateFile(pFile,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	return hFile;
#endif // !UNICODE
#endif //__LINUX__
}

SMKey ShareMemAPI::CreateShareMem(SMKey iKey, int vSize)
{
	if (iKey < 0)
	{
		LOG_ERROR("default", "CreateShareMem failed. iKey illegal.");
		return (SMKey)-1;
	}
#ifdef __LINUX__
	return shmget(iKey, (size_t)vSize, IPC_CREAT | IPC_EXCL | 0666);
#else
	CString<64> sKey(std::to_string((long long)iKey));
	return CreateFileMapping(
		(HANDLE)0xFFFFFFFFFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		vSize,
		sKey.c_str());
#endif
}

SMKey ShareMemAPI::OpenShareMem(SMKey iKey, int vSize)
{
	if (iKey < 0)
	{
		LOG_ERROR("default", "OpenShareMem failed. iKey illegal.");
		return (SMKey)-1;
	}
#ifdef __LINUX__
	iShmID = shmget(iKey, 0, 0666);
	return iShmID;
#else
	CString<64> sKey(std::to_string((long long)iKey));
	return OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		TRUE,
		sKey.c_str());
#endif
}

BYTE* ShareMemAPI::AttachShareMem(SMKey iKey)
{
	if (iKey < 0)
	{
		LOG_ERROR("default", "AttachShareMem failed. iKey illegal.");
		return NULL;
	}
#ifdef __LINUX__
	return (BYTE*)shmat(iShmID, NULL, 0);
#else
	CString<64> sKey(std::to_string((long long)iKey));
	return (BYTE*)MapViewOfFile(iKey,FILE_MAP_ALL_ACCESS,0,0,0);
#endif
}

int ShareMemAPI::DetachShareMem(BYTE* pAddr)
{
	if (pAddr == NULL)
	{
		LOG_ERROR("default", "DetachShareMem failed. pAddr illegal.");
		return 0;
	}
#ifdef __LINUX__
	return shmdt(pAddr);
#else
	return UnmapViewOfFile(pAddr);
#endif
}

int ShareMemAPI::DestroyShareMem(SMKey iKey)
{
	if (iKey < 0)
	{
		LOG_ERROR("default", "DestroyShareMem failed. iKey illegal.");
		return NULL;
	}
#ifdef __LINUX__
	return shmctl(iShmiKeyID, IPC_RMID, NULL);
#else
	return CloseHandle(iKey);
#endif
}