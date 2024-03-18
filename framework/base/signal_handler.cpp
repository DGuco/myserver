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
	//����ϵͳĬ�Ϻ���
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
		�ڵ��ý��̴���������ʱ���ں�ͨ�����ڽ���ջ��Ϊ�䴴��һջ��������������̶�ջ����չͻ���˶�ջ��С������ʱ�����������Ͳ�̫�����ˡ����磬ջ���������������ڻᴥ����һƬӳ���ڴ�������������Ķѣ��ֻ���ջ�Ĵ�С�Ѿ�ֱ��RLIMIT_STACK��Դ���ƣ���Щ���������������ķ�����
		�����̶�ջ����չ��ͼͻ��������ʱ���ں˽�Ϊ�ý��̲��� SIGSEGV�źźš���������Ϊջ�ռ���Ȼ�ľ����ں�Ҳ���޷�Ϊ�����Ѿ���װ��SIGSEGV ��������������ջ֡������ǣ������������ò������ã�������Ҳ����ֹ�ˡ�
		���ϣ�������������ȷ���� SIGSEGV �źŴ����������ĵ��ã�����Ҫ�����¹���
		����һ�鱻��Ϊ����ѡ�ź�ջ�����ڴ�������Ϊ�źŴ�������ջ֡
		����sigaltstack() ����֪�ں˸ñ�ѡ�ź�ջ�Ĵ���
		�ڴ����źŴ�����ʱָ��SA_ONSTACK ��־��Ҳ����֪ͨ�ں��ڱ�ѡջ��Ϊ��������������ջ֡��
		����ϵͳ���� sigaltstack()���ȿ��Դ���һ����ѡ�ź�ջ��Ҳ���Խ��Ѵ�����ѡ�ź�ջ�������Ϣ����
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

	//�����ź�
	//���ź����û��ն�����(�����������)����ʱ����,
	signal(SIGHUP, SIG_IGN);
	//�������(terminate)�ź�,ͨ������Ҫ������Լ������˳�
	signal(SIGTERM, SIG_IGN);
	//��������closeһ������ʱ����client�˽��ŷ����ݡ�����TCPЭ��Ĺ涨�����յ�һ��RST��Ӧ��client���������������������ʱ��ϵͳ�ᷢ��һ��SIGPIPE�źŸ�����
	signal(SIGPIPE, SIG_IGN);

	/*
	SA_NODEFER
	��Ҫ�ڴ������ִ��ʱ����ǰ�ź���ӵ��̵߳��ź������У������� act.sa_mask ��ָ���˸��źš� ��ˣ����߳�����ִ�д������ʱ�����Խ���ǰ�źŵ���һ��ʵ�����ݸ��̡߳� �˱�־���ڽ����źŴ������ʱ�������塣
	SA_ONSTACK
	�� sigaltstack(2) �ṩ�ı����źŶ�ջ�ϵ����źŴ������ ������ö�ջ�����ã���ʹ��Ĭ�϶�ջ�� �˱�־���ڽ����źŴ������ʱ�������塣�����ñ��ö�ջ��������Ҫ����sigaltstack(2) ����Ҫ���ô˱�־��
	SA_RESETHAND
	�ڽ����źŴ������ʱ���źŲ����ָ�ΪĬ��ֵ�� �˱�־���ڽ����źŴ������ʱ�������塣
	SA_SIGINFO
	�źŴ�������������������������һ���� ����������£�Ӧ���� sa_sigaction ������ sa_handler�� �˱�־���ڽ����źŴ������ʱ�������塣
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
