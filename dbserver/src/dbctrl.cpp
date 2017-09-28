//
// Created by DGuco on 17-7-13.
//

#include "../inc/dbctrl.h"
#include "../../framework/mem/sharemem.h"
#include "../../framework/json/config.h"
#include "../../framework/net/server_comm_engine.h"

CSharedMem* CDBCtrl::mShmPtr = NULL;
template<> CDBCtrl* CSingleton< CDBCtrl >::spSingleton = NULL;

CDBCtrl::CDBCtrl()
{
    m_iRunFlag = 0;
	m_which_handle = 0;
	m_lastTick = 0;
}

CDBCtrl::~CDBCtrl()
{
}

int CDBCtrl::MallocShareMem()
{
    char szCmd[ 128 ] = {0};
    snprintf(szCmd, sizeof(szCmd)-1, "touch %s", "./dbpipefile");
    system(szCmd);

    unsigned int tkeydb = MakeKey( "./dbpipefile", 'D' );
    size_t tSize = sizeof(CSharedMem) +  MAXHANDLENUMBER * CCodeQueue::CountQueueSize(INPUTQUEUELENGTH);
    BYTE* tpDBShm = CreateShareMem ( tkeydb, tSize);

    if (tpDBShm == NULL)
    {
        return -1;
    }
    LOG_DEBUG( "default", "DB Shm Size is %lld", tSize );
    CSharedMem::pbCurrentShm = tpDBShm;
    CDBCtrl::mShmPtr = CSharedMem::CreateInstance(tkeydb,tSize,SHM_INIT);
    if (CDBCtrl::mShmPtr == NULL)
    {
        return -1;
    }
    CDBHandle::ms_pCurrentShm = CDBCtrl::mShmPtr;
    return 0;
}

void CDBCtrl::SetRunFlag( int iFlag )
{
    m_iRunFlag = iFlag;
    LOG_INFO( "default", "Set Run Flag %d, All Flag Is %d", iFlag, m_iRunFlag );
}

void CDBCtrl::ClearRunFlag( int iFlag )
{
    m_iRunFlag = 0;
    LOG_INFO( "default", "Clear Run Flag %d", iFlag );
}

bool CDBCtrl::IsRunFlagSet( int iFlag )
{
    return iFlag == m_iRunFlag;
}

int CDBCtrl::Initialize( )
{

    int i;

    if(MallocShareMem() != 0)
    {
        return -1;
    }

    for( i = 0; i < MAXHANDLENUMBER; i++ )
    {
        CDBHandle* cdbHandle = new CDBHandle;
        if (!cdbHandle)
        {
            return -1;
        }
        m_apHandles[i] = cdbHandle;
    }

	return 0;
}


int  CDBCtrl::ConnectToProxyServer()
{
	int i = 0;

    //如果为null 让程序崩溃
	ServerInfo* proxyInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType ::FE_PROXYSERVER);
	m_stProxySvrdCon.Initialize( FE_PROXYSERVER,
								proxyInfo->m_iServerId,
								inet_addr(proxyInfo->m_sHost.c_str()),
								proxyInfo->m_iPort);

	if(m_stProxySvrdCon.ConnectToServer( (char*)proxyInfo->m_sHost.c_str()))
	{
		LOG_INFO( "default", "Error:connect to Proxy Server %d failed.\n", proxyInfo->m_iServerId);
		return -1;
	}

	if( RegisterToProxyServer() )
	{
		LOG_ERROR( "default", "Error: Register to Proxy Server %d failed.\n", proxyInfo->m_iServerId );
		return -1;
	}

	m_tLastSendKeepAlive = GetMSTime();	// 记录这一次的注册的时间
	m_tLastRecvKeepAlive = GetMSTime();	// 由于是注册,所以也将第一次收到的时间记录为当下

	LOG_INFO( "default", "Connect to Proxy server %d Succeed.\n", proxyInfo->m_iServerId);
	return i;
}

/********************************************************
Function:     RegisterToProxyServer
Description:  向proxy发送注册消息
Input:        nIndex:  连接下标
Output:      
Return:       0 :   成功 ，其他失败
Others:		
 ********************************************************/
int CDBCtrl::RegisterToProxyServer()
{
	CProxyMessage message;
	char message_buffer[1024] = {0};
	unsigned short tTotalLen = sizeof(message_buffer);

    //如果为null 让程序崩溃
    ServerInfo* dbInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType ::FE_DBSERVER);
	pbmsg_setproxy(message.mutable_msghead(), enServerType::FE_DBSERVER, dbInfo->m_iServerId,
				   enServerType::FE_PROXYSERVER, m_stProxySvrdCon.GetEntityID(), GetMSTime(), enMessageCmd ::MESS_REGIST);

	int iRet = ServerCommEngine::ConvertMsgToStream(&message, message_buffer, tTotalLen);
	if (iRet != 0)
	{
		LOG_ERROR("default", "CDBCtrl::RegisterToProxyServer ConvertMsgToStream failed, iRet = %d.", iRet);
		return 0;
	}

	iRet = m_stProxySvrdCon.GetSocket()->SendOneCode(tTotalLen, (BYTE*)message_buffer);
	if (iRet != 0)
	{
		LOG_ERROR("default", "CDBCtrl::RegisterToProxyServer SendOneCode failed, iRet = %d.", iRet);
		return -1;
	}

	LOG_INFO("default", "Regist to Proxy now.");

	return 0;
}

/********************************************************
Function:     SendkeepAliveToProxy
Description:  向proxy发送心跳消息
Input:        nIndex    连接指针
Output:      
Return:       0 :   成功 ，其他失败
Others:		
 ********************************************************/
int CDBCtrl::SendkeepAliveToProxy()
{
    CProxyMessage message;
	char message_buffer[1024] = {0};
	unsigned short tTotalLen = sizeof(message_buffer);

    //如果为null 让程序崩溃
    ServerInfo* dbInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_DBSERVER);
	pbmsg_setproxy(message.mutable_msghead(), enServerType::FE_DBSERVER,dbInfo->m_iServerId,
				   enServerType::FE_PROXYSERVER, m_stProxySvrdCon. GetEntityID(), GetMSTime(), enMessageCmd::MESS_KEEPALIVE);

	int iRet = ServerCommEngine::ConvertMsgToStream(&message, message_buffer, tTotalLen);
	if (iRet != 0)
	{
		LOG_ERROR("default", "CDBCtrsl::SendkeepAliveToProxy ConvertMsgToStream failed, iRet = %d.", iRet);
		return 0;
	}

	iRet = m_stProxySvrdCon.GetSocket()->SendOneCode(tTotalLen, (BYTE*)message_buffer);
	if (iRet != 0)
	{
		LOG_ERROR("default", "CDBCtrl::SendkeepAliveToProxy  proxy SendOneCode failed, iRet = %d.", iRet);
		return -1;
	}

	m_tLastSendKeepAlive = GetMSTime(); // 保存这一次的发送的时间

	LOG_INFO("default", "SendkeepAliveto Proxy now.");
	return 0;
}

// ***************************************************************
//  Function: 	DisPatchOneCode   
//  Description:分派一个消息
//  Date: 		10/09/2008
// 
// ***************************************************************
int CDBCtrl::DispatchOneCode(int nCodeLength, BYTE* pbyCode, bool vCountNum)
{
	int iTempRet = 0;
	int iHandleChoice = GetThisRoundHandle();
	// 解析proxy头
	CProxyHead tProxyHead;
	if ( ServerCommEngine::ConvertStreamToProxy(pbyCode + sizeof(int), nCodeLength - sizeof(int), &tProxyHead) < 0 )
	{
		LOG_ERROR( "default", "parse proxy head error!!!!!!!!!!!!!!!");
		return -1;
	}

	// 加入 proxy 命令处理处理心跳消息
	if( enServerType ::FE_PROXYSERVER == tProxyHead.srcfe() && enMessageCmd::MESS_KEEPALIVE == tProxyHead.opflag() )
	{
		m_tLastRecvKeepAlive = GetMSTime(); // 保存这一次的注册的时间
		return 0;
	}

	iTempRet = m_apHandles[iHandleChoice]->PostOneCode(nCodeLength, pbyCode);  // 追加一条待处理消息到相应的线程

	LOG_DEBUG( "default", "Post code to dbhandle_%d returns %d.", iHandleChoice, iTempRet);
	LOG_DEBUG( "default", "PRoxyHead:SrcFE: %d SrcID: %d DstFE: %d DstID: %d OpFlag: %d ", tProxyHead.srcfe(), tProxyHead.srcid(), tProxyHead.dstfe(), tProxyHead.dstid(),  tProxyHead.opflag() );
	if( iTempRet >= 0 )
	{
		m_apHandles[iHandleChoice]->WakeUp();  // 唤醒相应的线程
	}
	else
	{
		LOG_ERROR( "default", "Post code to dbhandle_%d returns %d.", iHandleChoice, iTempRet);
	}

	return iTempRet;
}

int CDBCtrl::CheckRunFlags()
{	
	int i;

	if( IsRunFlagSet( EFLG_CTRL_RELOAD ) )  // 如果 flag 设置为 reload ,停止所有处理 sql 的线程
	{
		for( i = 0; i < MAXHANDLENUMBER; i++ )
		{
			m_apHandles[i]->ProcessThreadEnd();
			m_apHandles[i]->InitLogFile( NULL, NULL, (LogLevel)CServerConfig::GetSingletonPtr()->GetDbLogLevel(), 10 * 1024 * 1024, 20 );
		}

		ClearRunFlag( EFLG_CTRL_RELOAD );
	}

	if( IsRunFlagSet( EFLG_CTRL_QUIT ) || IsRunFlagSet( EFLG_CTRL_SHUTDOWN ) )
	{

		LOG_INFO( "default", "Have got command to stop run, now stop threads ...");  // 从这行起,说明如何停止服务器

		// 停止所有的线程
		for( i = 0; i < MAXHANDLENUMBER; i++ )
		{
			m_apHandles[i]->ProcessThreadEnd();
			m_apHandles[i]->StopThread();
		}

		LOG_INFO("default", "All threads stopped, main control quit.");

		if( IsRunFlagSet( EFLG_CTRL_SHUTDOWN ) )
		{
			LOG_INFO("default", "gateserver resume,dbserver shutdown success!");
		}

		if( IsRunFlagSet( EFLG_CTRL_QUIT ) )
		{
			LOG_INFO("default", "dbserver shutdown success!");
		}

		exit(0);

	}
	return 0;
}

int CDBCtrl::CheckAndDispatchInputMsg()
{
	fd_set fds_read;
	struct timeval stMonTime;
	int i, iTempFD = -1, iTempStatus = tcs_closed;
	int iOpenFDNum = 0;
	int iCount = 0;
	unsigned short nTmpCodeLength;
	BYTE  abyCodeBuf[MAX_PACKAGE_LEN];

	FD_ZERO(&fds_read);
	stMonTime.tv_sec = 0;
	stMonTime.tv_usec = 10000;

    iTempFD = m_stProxySvrdCon.GetSocket()->GetSocketFD();
    iTempStatus = m_stProxySvrdCon.GetSocket()->GetStatus();

    if(iTempFD > 0 && iTempStatus == tcs_connected)
    {
        FD_SET(iTempFD, &fds_read);
    }

	iOpenFDNum = select(FD_SETSIZE, &fds_read, NULL, NULL, &stMonTime);  // 等待读取

	if(iOpenFDNum <= 0)  // 如果没有可读则退出
	{
		return 0;
	}


    iTempFD = m_stProxySvrdCon.GetSocket()->GetSocketFD();
    if(iTempFD < 0)
    {
        return -1;
    }


    if(FD_ISSET(iTempFD, &fds_read))
    {
        LOG_INFO( "default", "Proxy(index:%d,entityid:%d) had req to process.", i, m_stProxySvrdCon.GetEntityID());
        m_stProxySvrdCon.GetSocket()->RecvData();  // 接收数据到 m_abyRecvBuffer
        while(1)
        {
            nTmpCodeLength = sizeof(abyCodeBuf)/* - sizeof(int)*/;
            // 将单条消息接收到 abyCodeBuf
            if(!(m_stProxySvrdCon.GetSocket()->GetOneCode(nTmpCodeLength, (BYTE *)&abyCodeBuf[sizeof(int)]) > 0))
            {
                break;
            }

            LOG_DEBUG( "default", "Get one code from ( EntityType:%d, EntityID:%d ) Len=%d, dispatch it",
                    m_stProxySvrdCon.GetEntityType(),
                    m_stProxySvrdCon.GetEntityID(),
                    nTmpCodeLength);
//            memcpy((void *)abyCodeBuf, (const void *)&i, sizeof(int));  // 把 i 的值赋给 abyCodeBuf
//            nTmpCodeLength += sizeof(int);
            DispatchOneCode(nTmpCodeLength, (BYTE *)abyCodeBuf);  // 派发一个消息
            iCount++;
        }
    }

	return iCount;
}


int CDBCtrl::PrepareToRun()
{
	int i;

	if(ConnectToProxyServer() < 0)  // 连接到proxy服务器
	{
		LOG_ERROR( "default", "Error: in CDBCtrl::PrepareToRun connect proxy  server  failed!\n");
		return -1;
	}

	for(i = 0; i < MAXHANDLENUMBER; i++)
	{	
		m_apHandles[i]->Initialize(i, &m_stProxySvrdCon);
		m_apHandles[i]->InitLogFile( NULL, NULL, (LogLevel)CServerConfig::GetSingletonPtr()->GetDbLogLevel(), 10*1024*1024, 20, "dbhandle_" );
	}

	LOG_INFO( "default", "Handles initialized OK, now begin to create threads.\n");

	for(i = 0; i < MAXHANDLENUMBER; i++)  // 创建CDBHandle线程
	{
		if(m_apHandles[i]->CreateThread())
		{
			LOG_ERROR( "default", "Create dbhandle thread %d failed.\n", i);
			return -1;
		}
	}

	LOG_INFO( "default", "Successfully create %d threads to handle request.\n", MAXHANDLENUMBER);

	m_lastTick = GetMSTime();

	return 0;
}

int CDBCtrl::Run()
{
	while(True)
	{
		CheckRunFlags();  // 检查是否重新加载或者停止dbserver

		RoutineCheck();  // 用于保持和ProxyServer的连接

		CheckAndDispatchInputMsg();  // 检查和分发数据

	}

	return 0;
}

int CDBCtrl::RoutineCheck()
{
	time_t tNow = GetMSTime();

	if(tNow - m_lastTick < CServerConfig::GetSingletonPtr()->GetServetTick())
	{
		return 0;
	}

	// 和 proxy 保持心跳
    if( (m_stProxySvrdCon.GetSocket()->GetStatus() == tcs_connected)  // 已经连接
            && (m_stProxySvrdCon.GetSocket()->GetSocketFD() > 0)  // 文件描述符
            && ((tNow - m_tLastRecvKeepAlive) < (CServerConfig::GetSingletonPtr()->GetTcpKeepAlive() * 3)) // 与proxy心跳超时
      )
    {
        if ( tNow - m_tLastSendKeepAlive > CServerConfig::GetSingletonPtr()->GetTcpKeepAlive())
		{
			SendkeepAliveToProxy();
		}
		return 0;
    }

    LOG_ERROR("default", "Proxy(ID = %d) is not connected, try to reconnect it", m_stProxySvrdCon.GetEntityID());

    // 如果已经断开了,则重新连接
    if(m_stProxySvrdCon.ConnectToServer((char*)CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_PROXYSERVER)->m_sHost.c_str()))
    {
        LOG_ERROR("default", "Connect proxy failed.");
        return -1;
    }else{
        LOG_INFO("default", "Connect to proxy succeeded.");
    }

    // 然后注册
    if(RegisterToProxyServer())
    {
        LOG_ERROR("default", "Register proxyfailed.");
        return -1;
    }

    m_tLastSendKeepAlive = GetMSTime();	// 保存这一次的发送的时间
    m_tLastRecvKeepAlive = GetMSTime();	// 由于第一次发送,所以记录当前时间为接收的时间
	return 0;
}

int CDBCtrl::GetThisRoundHandle()
{
	int tWhich = m_which_handle++ ;
	if ( tWhich >= MAX_HANDLE_SZ )
	{
		m_which_handle = 0;
	}

	return tWhich;
}