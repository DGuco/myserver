#include "base.h"
#include "log.h"
#include "t_array.h"
#include "shm_api.h"

using namespace my_std;

namespace ShareMemAPI
{
// 	sm_key MakeKey(const char* pFile, int vId)
// 	{
// #ifdef __LINUX__
// 		return ftok(pFile, vId);
// #else
// #ifdef UNICODE
// 		TArray<wchar_t, 4096> wString;
// 		MultiByteToWideChar(CP_ACP, 0, pFile, -1, wString.GetData(), 4096);
// 		HANDLE hFile = CreateFile(wString,
// 			GENERIC_READ | GENERIC_WRITE,
// 			FILE_SHARE_READ,
// 			NULL,
// 			OPEN_EXISTING,
// 			FILE_ATTRIBUTE_NORMAL,
// 			NULL);
// #else
// 		HANDLE hFile = CreateFile(pFile,
// 			GENERIC_READ | GENERIC_WRITE,
// 			FILE_SHARE_READ,
// 			NULL,
// 			OPEN_EXISTING,
// 			FILE_ATTRIBUTE_NORMAL,
// 			NULL);
// 		return hFile;
// #endif // !UNICODE
// #endif //__LINUX__
// 	}

	sm_handler CreateShareMem(sm_key iKey, size_t vSize)
	{
		if (iKey < 0)
		{
			return (sm_handler)-1;
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

	sm_handler OpenShareMem(sm_key iKey, size_t vSize)
	{
		if (iKey < 0)
		{
			return (sm_handler)-1;
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

	BYTE* AttachShareMem(sm_handler iKey)
	{
		if (iKey < 0)
		{
			return NULL;
		}
#ifdef __LINUX__
		return (BYTE*)shmat(iKey, NULL, 0);
#else
		CString<64> sKey(std::to_string((long long)iKey));
		return (BYTE*)MapViewOfFile(iKey, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
	}

	bool DetachShareMem(BYTE* pAddr)
	{
		if (pAddr == NULL)
		{
			return false;
		}
#ifdef __LINUX__
		return shmdt(pAddr);
#else
		return UnmapViewOfFile(pAddr);
#endif
	}

	bool DestroyShareMem(sm_handler iKey)
	{
		if (iKey < 0)
		{
			return false;
		}
#ifdef __LINUX__
		return shmctl(iKey, IPC_RMID, NULL);
#else
		return CloseHandle(iKey);
#endif
	}
}