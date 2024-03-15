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

void DumpStack(const char* filename)
{
#ifdef __LINUX__
	void* DumpArray[25];
	int	Size = backtrace(DumpArray, 25);
	char** symbols = backtrace_symbols(DumpArray, Size);
	if (symbols)
	{
		if (Size > 10) Size = 10;
		if (Size > 0)
		{
			FILE* f = fopen(filename, "a");
			char threadinfo[256] = { 0 };
			sprintf(threadinfo, "thread_id = %d cause dump\r\n", MyGetCurrentThreadID());
			fwrite(threadinfo, 1, strlen(threadinfo), f);
			fwrite(type, 1, strlen(type), f);
			for (int i = 0; i < Size; i++)
			{
				printf("%s\r\n", symbols[i]);
				fwrite(symbols[i], 1, strlen(symbols[i]), f);
				fwrite("\r\n", 1, 2, f);
			}
			fclose(f);
		}
		free(symbols);
	}
	else
	{
		FILE* f = fopen(filename, "a");
		char	buffer[256] = { 0 };
		char threadinfo[256] = { 0 };
		sprintf(threadinfo, "thread_id = %d cause dump\r\n", MyGetCurrentThreadID());
		fwrite(threadinfo, 1, strlen(threadinfo), f);
		fwrite(type, 1, strlen(type), f);
		fclose(f);
	}
#endif
}