//
// Created by DGuco on 17-7-13.
//

#include "../../framework/net/server_comm_engine.h"
#include "code_queue.h"
#include "../../framework/message/dbmessage.pb.h"
#include "../../framework/base/base.h"
#include "../inc/dbhandle.h"
#include "../inc/dbctrl.h"
#include "../inc/queryresultmysql.h"
#include "../inc/databasemysql.h"
#include "../../framework/json/config.h"
#include "../../framework/net/client_comm_engine.h"

CSharedMem* CDBHandle::ms_pCurrentShm = NULL;
int CDBHandle::m_iDBSvrID = -1;
short CDBHandle::m_sDBOperMode = -1;
std::mutex CDBHandle::m_sMutex; 

CDBHandle::CDBHandle()
{
    // 初始化输入队列对象
    CCodeQueue::pCurrentShm = ms_pCurrentShm;
    m_pInputQueue = CCodeQueue::CreateInstance(INPUTQUEUELENGTH);
	if( m_pInputQueue == NULL )
	{
		printf( "in CDBHandle::CDBHandle(), new CCodeQueue failed" );
		exit(1);
	}

	m_iHandleID = -1;
    m_tLastCheckTime = time(NULL);
	m_pDatabase = new DatabaseMysql;
	if( m_pDatabase == NULL )
	{
		printf( "in CDBHandle::CDBHandle(), new DatabaseMysql failed" );
		exit(1);
	}

	mMsgFactory = new CMessageFactory;
	if( mMsgFactory == NULL )
	{
		printf( "in CDBHandle::CDBHandle(), new CMessageFactory failed" );
		exit(1);
	}
}

CDBHandle::~CDBHandle()
{
	if ( mMsgFactory != NULL )
	{
		delete mMsgFactory;
		mMsgFactory = NULL;
	}
	if ( m_pDatabase != NULL )
	{
		delete m_pDatabase;
		m_pDatabase = NULL;
	}
}

bool CDBHandle::IsToBeBlocked()
{
    if(!m_pInputQueue)  // 如果队列不存在
    {
        return True;
    }

    return m_pInputQueue->IsQueueEmpty();  // 不然则查看队列是否为空

}

int CDBHandle::Initialize(int iHandleID, CTCPConn<RECVBUFLENGTH, POSTBUFLENGTH>* pProxySvrdConns )
{
    m_iHandleID = iHandleID;
    m_stProxySvrdConns = *pProxySvrdConns;

	// 初始化日志
	InitLog( NULL, NULL, LEVEL_DEBUG );

    m_pDatabase->Initialize ( CServerConfig::GetSingleton().GetDbInfo().c_str(),
                              CServerConfig::GetSingleton().GetDbRwTimeout(),
                              CServerConfig::GetSingleton().GetDbSleep(),
                              CServerConfig::GetSingleton().GetDbLoop() );  // 初始化到mysql的连接


    return 0;
}

int CDBHandle::GetOneCode(int& nCodeLength, BYTE* pCode)
{
    int iTempRet = 0;

    if(!pCode)
    {
        return -1;
    }

	std::lock_guard<std::mutex> guard(m_sMutex);
    iTempRet = m_pInputQueue->GetHeadCode( pCode, &nCodeLength );
    return iTempRet;
}

int CDBHandle::PostOneCode(int nCodeLength, BYTE* pCode)
{
    int iTempRet = 0;

    if(!pCode || nCodeLength <= 0)
    {
        return -1;
    }

	std::lock_guard<std::mutex> guard(m_sMutex);
	iTempRet = m_pInputQueue->AppendOneCode((const BYTE *)pCode, nCodeLength);
	
    return iTempRet;
}

int CDBHandle::SendMessageTo(CProxyMessage *pMsg)
{
	if( pMsg == NULL)
	{
		TRACE_ERROR("in CDBHandle::SendMessageTo, pmsg is null");
		return -1;
	}

	CProxyHead *stProxyHead = pMsg->mutable_msghead();
	BYTE abyCodeBuf[MAX_PACKAGE_LEN] = {0};
	unsigned short nCodeLength = sizeof(abyCodeBuf);

	pbmsg_setproxy(stProxyHead,
                   m_stCurrentProxyHead.dstfe(),
                   m_stCurrentProxyHead.dstid(),
			       m_stCurrentProxyHead.srcfe(),
                   m_stCurrentProxyHead.srcid(),
                   GetMSTime(),
                   enMessageCmd::MESS_REGIST);

	int iRet = ServerCommEngine::ConvertMsgToStream(pMsg, abyCodeBuf, nCodeLength);
	if (iRet != 0)
	{
		LOG_ERROR("default", "CDBCtrl::RegisterToProxyServer ConvertMsgToStream failed, iRet = {}.", iRet);
		return 0;
	}
	
	//int nSendReturn = m_pProxySvrdConns[ m_current_proxy_index ].GetSocket()->SendOneCode( nCodeLength, abyCodeBuf );
	int nSendReturn = m_stProxySvrdConns.GetSocket()->SendOneCode( nCodeLength, abyCodeBuf );
	if( nSendReturn < 0 )
	{
		TRACE_ERROR( "Send Code(len:{}) To Proxy faild(error={})", nCodeLength, nSendReturn );
		return -1;
	}

	Message* pUnknownMessagePara = (Message*) pMsg->msgpara();
    MY_ASSERT( pUnknownMessagePara != NULL, return 0 );
	const ::google::protobuf::Descriptor* pDescriptor= pUnknownMessagePara->GetDescriptor();
	TRACE_DEBUG("SendMessageTo: MsgName[{}] ProxyHead[{}] MsgHead[{}] MsgPara[{}]",
			pDescriptor->name().c_str(),stProxyHead->ShortDebugString().c_str(),
			pMsg->ShortDebugString().c_str(), ((Message*) pMsg->msgpara())->ShortDebugString().c_str());

	return 0;
}

int CDBHandle::ConnectToLocalDB()
{
    bool bInitRet =  m_pDatabase->Initialize ( CServerConfig::GetSingleton().GetDbInfo().c_str(),
                                  CServerConfig::GetSingleton().GetDbRwTimeout(),
                                  CServerConfig::GetSingleton().GetDbSleep(),
                                  CServerConfig::GetSingleton().GetDbLoop() );  // 初始化到mysql的连接
	return bInitRet ? 0 : -1;
}

int CDBHandle::InitLogFile( const char* vLogName, const char* vLogDir, LogLevel vPriority  /*= LEVEL_NOTSET*/ , unsigned int vMaxFileSize  /*= 10*1024*1024*/ , unsigned int vMaxBackupIndex  /*= 1*/ , bool vAppend  /*= true*/  )
{
	TFName szThreadLogFile;

	// 设置服务器日志
	snprintf( szThreadLogFile, sizeof(szThreadLogFile)-1,"../log/dbhandle{}.log", m_iHandleID);

	ThreadLogInit( szThreadLogFile, vMaxFileSize, vMaxBackupIndex, 0, vPriority);

    MY_ASSERT( m_pDatabase != NULL, return -1 );

	m_pDatabase->InitLog( szThreadLogFile, szThreadLogFile, vPriority, vMaxFileSize,vMaxBackupIndex );

	return 0;
}

// 执行相应的指令
int CDBHandle::Event(CProxyMessage *pMsg)
{
	if( pMsg == NULL )
	{
		TRACE_ERROR("In CDBHandle::Event, input msg null" );
		return -1;
	}

	Message* pUnknownMessagePara = (Message*) pMsg->msgpara();
	MY_ASSERT( pUnknownMessagePara != NULL, return 0 );
	const ::google::protobuf::Descriptor* pDescriptor= pUnknownMessagePara->GetDescriptor();
	TRACE_DEBUG("ReceveMsg: MsgName[{}] MsgHead[{}] MsgPara[{}]",
			pDescriptor->name().c_str(), pMsg->ShortDebugString().c_str(), ((Message*) pMsg->msgpara())->ShortDebugString().c_str());

	switch ( pMsg->msghead().messageid() )
	{
		case CMsgExecuteSqlRequest::MsgID:  // 服务器执行SQL请求
			{
				ProcessExecuteSqlRequest( pMsg );
				break;
			}
		default:
			{
				break;
			}
	}

	return 0;
}

int CDBHandle::PrepareToRun()
{
	TRACE_DEBUG("Handle {} prepare to run.\n", m_iHandleID);

	if(ConnectToLocalDB())  // 真正的数据库连接已经在线程建立之前完成
	{
		TRACE_ERROR("In CDBHandle::PrepareToRun, Connect to database failed.\n");
		return -1;
	}

	return 0;
}

int CDBHandle::RunFunc()
{

	BYTE abyCodeBuf[MAX_PACKAGE_LEN];
	int nTempCodeLength = sizeof(abyCodeBuf);
	CProxyMessage stTempMsg;

	while(True)
	{
		CondBlock();  // 线程阻塞

		int iTempRet = GetOneCode(nTempCodeLength, (BYTE *)abyCodeBuf);  // 从队列中取出 Code
		if(iTempRet < 0)
		{
			TRACE_ERROR("Get one code from input queue returns {}.\n", iTempRet);
			continue;
		}

		if(nTempCodeLength <= 0)
		{
			TRACE_ERROR("Got one empty code from input queue, ignore it.\n");
			continue;
		}

		if(nTempCodeLength <= (int)sizeof(int))  // 对于空 Code ，丢弃
		{
			TRACE_ERROR("Got one empty code from input queue, ignore it.\n");
			continue;
		}

		// 将解析出的消息头和消息体分别存放在 m_stCurrentProxyHead stTempMsg
		int tRet = ServerCommEngine::ConvertStreamToMsg( (BYTE*)(abyCodeBuf/*+sizeof(int)*/),
                                                         nTempCodeLength/*-sizeof(int)*/,
                                                         &stTempMsg,
                                                         mMsgFactory);
        CProxyHead tmpProxyHead = stTempMsg.msghead();
        pbmsg_setproxy(&m_stCurrentProxyHead,
                       tmpProxyHead.srcfe(),
                       tmpProxyHead.srcid(),
                       tmpProxyHead.dstfe(),
                       tmpProxyHead.dstid(),
                       GetMSTime(),
                       enMessageCmd::MESS_REGIST);
		if( tRet != 0 )  // 如果解析失败则重新取 Code
		{  
			TRACE_ERROR("Convert code to message failed. tRet = {}", tRet);
			continue;
		}

		iTempRet = Event( &stTempMsg );  // 服务器执行相应的 Msg ，其实就是执行 SQL

		if(iTempRet)
		{
			TRACE_ERROR("Handle event returns {}.\n", iTempRet);
		}

		// 消息回收
		Message* pMessagePara = (Message*)(stTempMsg.msgpara());
		if (  pMessagePara )
		{
			pMessagePara->~Message();
			stTempMsg.set_msgpara((unsigned long)NULL);
		}
	}

	return 0;
}