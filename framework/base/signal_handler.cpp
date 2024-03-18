#include "signal_handler.h"
#include "log.h"
#include "platform_def.h"

#ifdef __LINUX__
struct stSinInfo
{
	int			sig_;
	const char* signame_;
};

static char g_MyStackMem[SIGSTKSZ];
static stSinInfo g_SigInfoList[] =
{
	{SIGHUP,"SIGHUP"},
	{SIGINI,,"SIGINI"},
	{SIGQUIT,,"SIGQUIT"},
	{SIGILL,"SIGILL"},
	{SIGABRT,"SIGABRT"},
	{SIGFPE,"SIGFPE"},
	{SIGKILL,"SIGKILL"},
	{SIGSEGV,"SIGSEGV"},
	{SIGPIPE,"SIGPIPE"},
	{SIGALRM,"SIGALRM"},
	{SIGTERM,"SIGTERM"},
	{SIGUSR1,"SIGUSR1"},
	{SIGUSR2,"SIGUSR2"},
	{SIGCHLD,"SIGCHLD"},
	{SIGCONT,"SIGCONT"},
	{SIGSTOP,"SIGSTOP"},
	{SIGTSTP,"SIGTSTP"},
	{SIGTTIN,"SIGTTIN"},
	{SIGTTOU,"SIGTTOU"},
	{SIGBUS,"SIGBUS"},
	{SIGPROF,"SIGPROF"},
	{SIGSYS,"SIGSYS"},
	{SIGTRAP,"SIGTRAP"},
	{SIGURG,"SIGURG"},
	{SIGVTALRM,"SIGVTALRM"},
	{SIGXCPU,"SIGXCPU"},
	{SIGXFSZ,"SIGXFSZ"},
	{SIGSTKFLT,"SIGSTKFLT"},
	{SIGIO,"SIGIO"},
	{SIGPWR,"SIGPWR"},
	{SIGWINCH,"SIGWINCH"},
};

void SigHandler(int sig, siginfo_t* siginfo, void* data)
{
	char* signame = "";
	for (int index = 0; index < sizeof(g_SigInfoList) / sizeof(stSinInfo); index++)
	{
		if (siginfo->sig_ == sig)
		{
			signame = siginfo->signame_;
			break;
		}
	}

	CSignalHandler::GetSingletonPtr()->DumpLog("------------------CoreDump Start------------------")
	CSignalHandler::GetSingletonPtr()->DumpStack(signame);
	CSignalHandler::GetSingletonPtr()->DumpLog("-------------------Core Dump End------------------")
	//设置系统默认函数
	struct sigaction sigopt;
	sigemptyset(&sigopt.sa_mask)
	act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
	act.sa_handler = SIG_DFL;
	sigaction(sig, &sigopt, NULL);
	kill(getpid(), sig);
}

#endif


CSignalHandler::CSignalHandler()
{}

CSignalHandler::~CSignalHandler()
{}

void CSignalHandler::RegisterHandler(std::string modulename_)
{
	moduleName = modulename_;
#ifdef __LINUX__
	/*
		在调用进程处理器函数时，内核通常会在进程栈中为其创建一栈。不过，如果进程对栈的扩展突破了对栈大小的限制时，这种做法就不太可行了。比如，栈的增长过大，以至于会触及到一片映射内存或者向上增长的堆，又或者栈的大小已经直逼RLIMIT_STACK资源限制，这些都会造成这种情况的发生。
		当进程对栈的扩展试图突破其上限时，内核将为该进程产生 SIGSEGV信号号。不过，因为栈空间已然耗尽，内核也就无法为进程已经安装的SIGSEGV 处理器函数创建栈帧。结果是，处理器函数得不到调用，而进程也就终止了。
		如果希望在这种情况下确保对 SIGSEGV 信号处理器函数的调用，就需要做如下工作
		分配一块被称为“备选信号栈”的内存区域，作为信号处理函数的栈帧
		调用sigaltstack() ，告知内核该备选信号栈的存在
		在创建信号处理函数时指定SA_ONSTACK 标志，也就是通知内核在备选栈上为处理器函数创建栈帧。
		利用系统调用 sigaltstack()，既可以创建一个备选信号栈，也可以将已创建备选信号栈的相关信息返回
	*/
	stack_t ss;
	ss.ss_sp = g_MyStackMem;
	ss.ss_size = sizeof(g_MyStackMem);
	ss.ss_flags = 0;
	if (sigaltstack(&ss, NULL))
	{
		DISK_LOG(ERROR_DISK, "sigaltstack failed");
		exit(1);
	}

	//忽略信号
	//本信号在用户终端连接(正常或非正常)结束时发出,
	signal(SIGHUP, SIG_IGN);
	//程序结束(terminate)信号,通常用来要求程序自己正常退出
	signal(SIGTERM, SIG_IGN);
	//当服务器close一个连接时，若client端接着发数据。根据TCP协议的规定，会收到一个RST响应，client再往这个服务器发送数据时，系统会发出一个SIGPIPE信号给进程
	signal(SIGPIPE, SIG_IGN);

	/*
	SA_NODEFER
	不要在处理程序执行时将当前信号添加到线程的信号掩码中，除非在 act.sa_mask 中指定了该信号。 因此，当线程正在执行处理程序时，可以将当前信号的另一个实例传递给线程。 此标志仅在建立信号处理程序时才有意义。
	SA_ONSTACK
	在 sigaltstack(2) 提供的备用信号堆栈上调用信号处理程序。 如果备用堆栈不可用，则将使用默认堆栈。 此标志仅在建立信号处理程序时才有意义。若想用备用堆栈，不仅需要调用sigaltstack(2) ，还要设置此标志。
	SA_RESETHAND
	在进入信号处理程序时将信号操作恢复为默认值。 此标志仅在建立信号处理程序时才有意义。
	SA_SIGINFO
	信号处理程序接受三个参数，而不是一个。 在这种情况下，应设置 sa_sigaction 而不是 sa_handler。 此标志仅在建立信号处理程序时才有意义。
	*/
	struct sigaction sigopt;
	sigopt.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
	sigopt.sa_sigaction = SigHandler;

	sigaction(SIGINT, &sigopt, NULL);
	sigaction(SIGQUIT, &sigopt, NULL);
	sigaction(SIGILL, &sigopt, NULL);
	sigaction(SIGABRT, &sigopt, NULL);
	sigaction(SIGFPE, &sigopt, NULL);
	sigaction(SIGSEGV, &sigopt, NULL);
	sigaction(SIGBUS, &sigopt, NULL);
	sigaction(SIGPROF, &sigopt, NULL);
	sigaction(SIGSYS, &sigopt, NULL);
	sigaction(SIGTRAP, &sigopt, NULL);
	sigaction(SIGXFSZ, &sigopt, NULL);
	sigaction(SIGPWR, &sigopt, NULL);
#endif
	return;
}

void CSignalHandler::DumpStack(const char* sigtype)
{
#ifdef __LINUX__
	if (sigtype == NULL)
	{
		return;
	}
	void* DumpArray[25];
	int	Size = backtrace(DumpArray, 25);
	char** symbols = backtrace_symbols(DumpArray, Size);
	if (symbols)
	{
		if (Size > 10) Size = 10;
		if (Size > 0)
		{
			FILE* f = fopen(DumpFileName().c_str(), "a");
			char threadinfo[256] = { 0 };
			sprintf(threadinfo, "thread = %lld raise signal,sigtype = %s \r\n", MyGetCurrentThreadID(), sigtype);
			fwrite(threadinfo, 1, strlen(threadinfo), f);
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
		FILE* f = fopen(DumpFileName().c_str(), "a");
		char	buffer[256] = { 0 };
		char threadinfo[256] = { 0 };
		sprintf(threadinfo, "thread = %lld raise signal,sigtype = %s \r\n", MyGetCurrentThreadID(), sigtype);
		fwrite(sigtype, 1, strlen(sigtype), f);
		fclose(f);
	}
#endif
}

std::string CSignalHandler::DumpFileName()
{
	std::string filename = "../log/";
	filename = filename + moduleName + "_dump.log";
	return filename;
}

void CSignalHandler::DumpLog(const char* msg)
{
	if ( msg == NULL)
	{
		return;
	}
	FILE* f = fopen(DumpFileName().c_str(), "a");
	fwrite(msg, 1, strlen(msg), f);
	fclose(f);
}
