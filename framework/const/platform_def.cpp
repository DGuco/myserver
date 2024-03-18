#include "platform_def.h"

#ifdef __LINUX__

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
#endif


TID  MyGetCurrentThreadID()
{
#if defined(__WINDOWS__)
		return GetCurrentThreadId();
#elif defined(__LINUX__)
		return pthread_self();
#endif
}