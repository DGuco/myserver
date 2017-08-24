//
// Created by DGuco on 17-7-12.
//

#include "../framework/mem/sharemem.h"
#include "../framework/mem/codequeue.h"
#include "./inc/dbctrl.h"
#include "./inc/basedb.h"
#include <unistd.h>
#include <signal.h>

int Initialize(int iInitFlag = 0);

void sigusr1_handle( int iSigVal )
{
	CDBCtrl::GetSingletonPtr()->SetRunFlag( CDBCtrl::EFLG_CTRL_RELOAD );
    signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle( int iSigVal )
{
	CDBCtrl::GetSingletonPtr()->SetRunFlag( CDBCtrl::EFLG_CTRL_QUIT );
    signal(SIGUSR2, sigusr2_handle);
}

void sigusr_handle( int iSigVal )
{
	CDBCtrl::GetSingletonPtr()->SetRunFlag( CDBCtrl::EFLG_CTRL_SHUTDOWN );
    signal(SIGQUIT, sigusr_handle);
}

void ignore_pipe()
{
    struct sigaction sig;

    sig.sa_handler = SIG_IGN;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGPIPE,&sig,NULL);
}


int Initialize(int iInitFlag )
{
	char szCmd[ 128 ] = {0};
	snprintf(szCmd, sizeof(szCmd)-1, "touch %s", "./dbpipefile");
	system(szCmd);

	unsigned int tkeydb = MakeKey( "./dbpipefile", 'D' );
	size_t tSize = CDBCtrl::CountSize() + sizeof( CSharedMem );
	BYTE* tpDBShm = CreateShareMem ( tkeydb, tSize );

	MY_ASSERT( ( tpDBShm != NULL ), return -1 );

	LOG_DEBUG( "default", "DB Shm Size is %lld", tSize );

	CSharedMem::pbCurrentShm = tpDBShm;
	CDBCtrl::mShmPtr = new CSharedMem( tkeydb, tSize, iInitFlag );
	YQ_ASSERT( CDBCtrl::mShmPtr != NULL, return -1 );

    return 0;
}


int main(int argc, char **argv)
{
    int iInitFlag = 1;
	int iInitDaemon = SERVER_INIT_DAEMON;

    INIT_ROLLINGFILE_LOG( "default", "../log/dbserver.log", LEVEL_DEBUG, 10 * 1024 * 1024, 20 );
//	INIT_ROLLINGFILE_LOG( "dbctrl", "../log/dbserver.log", LEVEL_DEBUG, 10 * 1024 * 1024, 20 );

	if( argc > 1 )
	{
		// 支持版本信息查询
		if( !strcasecmp(argv[1], "-v") )
		{
#ifdef _DEBUG_
			printf("mhdl dbserver debug build at %s %s\n", __DATE__, __TIME__);
#else
			printf("mhdl dbserver release build at %s %s\n", __DATE__, __TIME__);
#endif
			exit(0);
		}

		int i = 0;
		for( i = 1; i < argc; i++ )
		{
			if( strcasecmp((const char *)argv[i], "-Init") == 0 )
			{
				iInitFlag = 1;
			}
			else if( strcasecmp((const char *)argv[i], "-r") == 0  )
			{
				iInitFlag = 0;
				printf( "dataserver can not run in resume model ...\n" );
				exit(0);
			}
			else if( strcasecmp((const char *)argv[i], "-d") == 0 )
			{
				 iInitDaemon = SERVER_INIT_UNDAEMON;	
			}
		}
	}

    if( Initialize(iInitFlag) < 0 )  // 创建共享内存
    {
        exit(-1);
    }

	unique_ptr<CServerConfig> pTmpConfig(new CServerConfig);
    const string filepath = "../config/serverinfo.json";
    if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filepath))
    {
        LOG_ERROR("default","Get TcpserverConfig failed");
        exit(0);
    }

	CDBCtrl* tpDBCtrl = new CDBCtrl;
	if( tpDBCtrl->Initialize () < 0 )  // 读取数据库配置文件
	{
		exit(-1);
	}

    if( tpDBCtrl->PrepareToRun() < 0 )  // 创建处理线程
    {
        exit(-1);
	}

	signal(SIGUSR1, sigusr1_handle);
	signal(SIGUSR2, sigusr2_handle);
	signal(SIGQUIT, sigusr_handle);

	{
		printf( "dataserver begin to run ...\n" );
	}

	tpDBCtrl->Run();

	if (tpDBCtrl)
	{
		delete tpDBCtrl;
		tpDBCtrl = NULL;
	}

	// 关闭所有日志
	LOG_SHUTDOWN_ALL;

    return 0;
}

