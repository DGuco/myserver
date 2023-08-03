#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifdef __LINUX__
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#endif
#include "platform_def.h"
#include "share_mem.h"
#include "log.h"


SMKey ShareMemAPI::MakeKey(const char *pFile, int vId)
{
#ifdef __LINUX__
	return ftok(pFile, vId);
#else
#ifdef UNICODE
	wchar_t wString[4096] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, pFile, -1, wString, 4096);
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
#endif
}

BYTE * ShareMemAPI::CreateShareMem(SMKey iKey, int vSize)
{
#ifdef __LINUX__
	if (iKey < 0) 
	{
		LOG_ERROR("default", "CreateShareMem failed. iKey illegal.");
		return NULL;
	}

	iShmID = shmget(iKey, (size_t)vSize, IPC_CREAT | IPC_EXCL | 0666);
	return iShmID;
	if (iShmID < 0)
	{
		if (errno != EEXIST) {
			LOG_ERROR("default", "Alloc share memory failed, iKey:{}, size:{}, error:{}",
				iKey, iTempShmSize, strerror(errno));
			exit(-1);
		}
		LOG_INFO("default", "Same shm seg (key={}) exist, now try to attach it...", iKey);
		iShmID = shmget(iKey, iTempShmSize, IPC_CREAT | 0666);
		if (iShmID < 0) {
			LOG_INFO("default", "Attach to share memory {} failed, {}. Now try to touch it", iShmID, strerror(errno));
			iShmID = shmget(iKey, 0, 0666);
			if (iShmID < 0) {
				LOG_ERROR("default", "Fatel error, touch to shm failed, {}.", strerror(errno));
				exit(-1);
			}
			else {
				LOG_INFO("default", "First remove the exist share memory {}", iShmID);
				if (shmctl(iShmID, IPC_RMID, NULL)) {
					LOG_ERROR("default", "Remove share memory failed, {}", strerror(errno));
					exit(-1);
				}
				iShmID = shmget(iKey, iTempShmSize, IPC_CREAT | IPC_EXCL | 0666);
				if (iShmID < 0) {
					LOG_ERROR("default", "Fatal error, alloc share memory failed, {}", strerror(errno));
					exit(-1);
				}
			}
		}
		else {
			LOG_INFO("default", "Attach to share memory succeed.");
		}
	}

	LOG_INFO("default", "Successfully alloced share memory block, (key={}), id = {}, size = {}", iKey, iShmID, iTempShmSize);
	BYTE* tpShm = (BYTE*)shmat(iShmID, NULL, 0);
	if ((void*)-1 == tpShm) {
		LOG_ERROR("default", "create share memory failed, shmat failed, iShmID = {}, error = {}.",
			iShmID, strerror(errno));
		exit(0);
	}

	return tpShm;
#else
	ctx->map = CreateFileMapping(
		ctx->handle,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL
	);
#endif // __LINUX__
}

int ShareMemAPI::DestroyShareMem(SMKey iKey)
{
	int iShmID;

	if (iKey < 0) {
		LOG_ERROR("default", "Error in ftok, {}.", strerror(errno));
		return -1;
	}
	LOG_INFO("default", "Touch to share memory key = {}...", iKey);
	iShmID = shmget(iKey, 0, 0666);
	if (iShmID < 0) {
		LOG_ERROR("default", "Error, touch to shm failed, {}", strerror(errno));
		return -1;
	}
	else {
		LOG_INFO("default", "Now remove the exist share memory {}", iShmID);
		if (shmctl(iShmID, IPC_RMID, NULL)) {
			LOG_ERROR("default", "Remove share memory failed, {}", strerror(errno));
			return -1;
		}
		LOG_INFO("default", "Remove shared memory(id = {}, key = {}) succeed.", iShmID, iKey);
	}
	return 0;
}