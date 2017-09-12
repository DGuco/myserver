//
// Created by DGuco on 17-7-13.
//

#include "../../framework/message/server_comm_engine.h"
#include "../../framework/mem/codequeue.h"
#include "../../framework/message/dbmessage.pb.h"
#include "../inc/dbhandle.h"
#include "../inc/dbctrl.h"
#include "../inc/queryresultmysql.h"
#include "../inc/databasemysql.h"
#include "../../framework/json/config.h"

CSharedMem* CDBHandle::ms_pCurrentShm = NULL;
int CDBHandle::m_iDBSvrID = -1;
short CDBHandle::m_sDBOperMode = -1;
std::mutex CDBHandle::m_sMutex; 

CDBHandle::CDBHandle()
{
    if(!ms_pCurrentShm)
    {
        return ;
    }

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

    ServerInfo dbInfo = CServerConfig::GetSingleton().GetServerMap().find(enServerType::FE_DBSERVER)->second;

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
		LOG_ERROR("default", "CDBCtrl::RegisterToProxyServer ConvertMsgToStream failed, iRet = %d.", iRet);
		return 0;
	}
	
	//int nSendReturn = m_pProxySvrdConns[ m_current_proxy_index ].GetSocket()->SendOneCode( nCodeLength, abyCodeBuf );
	int nSendReturn = m_stProxySvrdConns.GetSocket()->SendOneCode( nCodeLength, abyCodeBuf );
	if( nSendReturn < 0 )
	{
		TRACE_ERROR( "Send Code(len:%d) To Proxy faild(error=%d)", nCodeLength, nSendReturn );
		return -1;
	}

	Message* pUnknownMessagePara = (Message*) pMsg->msgpara();
    MY_ASSERT( pUnknownMessagePara != NULL, return 0 );
	const ::google::protobuf::Descriptor* pDescriptor= pUnknownMessagePara->GetDescriptor();
	TRACE_DEBUG("SendMessageTo: MsgName[%s] ProxyHead[%s] MsgHead[%s] MsgPara[%s]",
			pDescriptor->name().c_str(),stProxyHead->ShortDebugString().c_str(),
			pMsg->ShortDebugString().c_str(), ((Message*) pMsg->msgpara())->ShortDebugString().c_str());

	return 0;
}

int CDBHandle::ConnectToLocalDB()
{
    bool bInitRet=  m_pDatabase->Initialize ( CServerConfig::GetSingleton().GetDbInfo().c_str(),
                                  CServerConfig::GetSingleton().GetDbRwTimeout(),
                                  CServerConfig::GetSingleton().GetDbSleep(),
                                  CServerConfig::GetSingleton().GetDbLoop() );  // 初始化到mysql的连接
	return bInitRet ? 0 : -1;
}

int CDBHandle::InitLogFile( const char* vLogName, const char* vLogDir, LogLevel vPriority  /*= LEVEL_NOTSET*/ , unsigned int vMaxFileSize  /*= 10*1024*1024*/ , unsigned int vMaxBackupIndex  /*= 1*/ , bool vAppend  /*= true*/  )
{
	TFName szThreadLogFile;

	// 设置服务器日志
	snprintf( szThreadLogFile, sizeof(szThreadLogFile)-1,"../log/dbhandle%d.log", m_iHandleID);

	ThreadLogInit( szThreadLogFile, vMaxFileSize, vMaxBackupIndex, 0, vPriority);

    MY_ASSERT( m_pDatabase != NULL, return -1 );

	m_pDatabase->InitLog( szThreadLogFile, szThreadLogFile, vPriority, vMaxFileSize,vMaxBackupIndex );

	return 0;
}

int CDBHandle::ProcessThreadEnd()
{
	TRACE_DEBUG("---Handle End Process Begin---\n");
	/* ProcessCheckPonits();*/
	TRACE_DEBUG("---Handle End Process End---\n");
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
	TRACE_DEBUG("ReceveMsg: MsgName[%s] MsgHead[%s] MsgPara[%s]",
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
	TRACE_DEBUG("Handle %d prepare to run.\n", m_iHandleID);

	if(ConnectToLocalDB())  // 真正的数据库连接已经在线程建立之前完成
	{
		TRACE_ERROR("In CDBHandle::PrepareToRun, Connect to database failed.\n");
		return -1;
	}

	return 0;
}

int CDBHandle::Run()
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
			TRACE_ERROR("Get one code from input queue returns %d.\n", iTempRet);
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
		int tRet = ServerCommEngine::ConvertStreamToMsg( (BYTE*)(abyCodeBuf+sizeof(int)),
                                                         nTempCodeLength-sizeof(int),
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
			TRACE_ERROR("Convert code to message failed. tRet = %d", tRet);
			continue;
		}

		iTempRet = Event( &stTempMsg );  // 服务器执行相应的 Msg ，其实就是执行 SQL

		if(iTempRet)
		{
			TRACE_ERROR("Handle event returns %d.\n", iTempRet);
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

// 执行相应的指令
int CDBHandle::ProcessExecuteSqlRequest( CProxyMessage* pMsg )
{
	if( pMsg == NULL )
	{
		TRACE_ERROR( "Error: [%s][%d][%s], invalid input.\n", __MY_FILE__, __LINE__, __FUNCTION__ );
		return -1;
	}

	CMsgExecuteSqlRequest* pReqMsg = (CMsgExecuteSqlRequest*)( pMsg->msgpara() );

	if( pReqMsg == NULL )
	{
		TRACE_ERROR( "Error: [%s][%d][%s], msgpara null.\n", __MY_FILE__, __LINE__, __FUNCTION__ );
		return -1;
	}

	if( pReqMsg->sql().length() <= 0 )
	{
		TRACE_ERROR( "Error: [%s][%d][%s], sql len(%d) invalid.\n", __MY_FILE__, __LINE__, __FUNCTION__, pReqMsg->sql().length()  );
		return -1 ;
	}


	TRACE_DEBUG( "%s \n", pReqMsg->ShortDebugString().c_str() );

	string sqlStr="";
	if ( pReqMsg->hasblob() == HASBLOB)
	{
		// 有blob字段的处理
		sqlStr += pReqMsg->sql();
		if ( pReqMsg->bufsize() > 0 )
		{
			if( pReqMsg->bufsize() >=  MAX_PACKAGE_LEN )
			{
				// blob字段超出长度
				TRACE_ERROR( "Error: [%s][%d][%s], exec sql %s faild. sql_len:%d > MAX_PACKAGE_LEN\n", __MY_FILE__, __LINE__, __FUNCTION__, pReqMsg->sql().c_str(), pReqMsg->bufsize() );
				return -1;
			}

			char sqlBuff[MAX_PACKAGE_LEN*2+1] = {0};
			int len = m_pDatabase->escape_string(sqlBuff, pReqMsg->buffer().c_str(), pReqMsg->bufsize());
			if ( len <= 0 )
			{
				TRACE_ERROR( "Error: [%s][%d][%s], escape_string error!!!!\n", __MY_FILE__, __LINE__, __FUNCTION__ );
				return -1;
			}
			sqlStr += sqlBuff;
		}
		sqlStr += pReqMsg->sqlwhere();
	}
	else 
	{
		// 没有blob字段的处理
		sqlStr += pReqMsg->sql();
	}

	// 需要回执
	if( MUSTCALLBACK == pReqMsg->callback() )
	{
		CProxyMessage tMsg;  // 该消息为回执
		CMsgExecuteSqlResponse tSqlResMsg;  // tSqlResMsg 为消息体
		tMsg.Clear();
		tSqlResMsg.Clear();

		tSqlResMsg.set_logictype( pReqMsg->logictype() );
		tSqlResMsg.set_sessionid( pReqMsg->sessionid() );
		tSqlResMsg.set_timestamp( pReqMsg->timestamp() );
		tSqlResMsg.set_teamid( pReqMsg->teamid() );

		TRACE_DEBUG("Execute SQL: %s", sqlStr.c_str());	// 用于在日志中查看SQL查询语句

		tMsg.mutable_msghead()->set_messageid( CMsgExecuteSqlResponse::MsgID );
		tMsg.set_msgpara( (uint64)&tSqlResMsg );

		if( pReqMsg->sqltype() == SELECT || pReqMsg->sqltype() == CALL )
		{
			QueryResult* res = NULL;
			if ( pReqMsg->sqltype() == SELECT )
			{
				res = m_pDatabase->Query( sqlStr.c_str(), sqlStr.length());
			}
			else
			{
				res = m_pDatabase->QueryForprocedure( sqlStr.c_str(), sqlStr.length(), pReqMsg->outnumber());
			}
			if( res == NULL )  // 无返回结果
			{
				TRACE_DEBUG( "sql %s exec, but no resultset returned", sqlStr.c_str());
				tSqlResMsg.set_resultcode( 0 );
			}
			else  // 有返回结果
			{
				tSqlResMsg.set_resultcode( 1 );
				tSqlResMsg.set_rowcount( res->GetRowCount() );  // 行
				tSqlResMsg.set_colcount( res->GetFieldCount() );  // 列

				//TODO: 列名 + 列类型 暂时不赋值
				if( res->GetRowCount() > 0 )
				{
					// 列值 和 值长度
					do
					{
						Field* pField = res->Fetch();
						if( pField == NULL )
						{
							TRACE_ERROR("ERROR: do sql %s success, row[%d], col[%d], but some row is null\n",sqlStr.c_str(), res->GetRowCount(), res->GetFieldCount() );

							//TODO: 出错设为0
							tSqlResMsg.set_rowcount( 0 );
							break;
						}

						for( int j = 0; j < (int)res->GetFieldCount(); j++ )
						{
							tSqlResMsg.add_fieldvalue( pField[j].GetString(), pField[j].GetValueLen() );
							tSqlResMsg.add_fieldvaluelen( pField[j].GetValueLen() );
						}

					}while(  res->NextRow() );
				}
			}
			
			// 安全释放结果集
			ReleaseResult(res);
		}
		else  // 非select操作
		{
			bool bExecResult = m_pDatabase->RealDirectExecute( sqlStr.c_str(), sqlStr.length() );  // 执行操作
			tSqlResMsg.set_resultcode( bExecResult ? 1 : 0 );
		}

		// 回复客户端
		SendMessageTo( &tMsg );

	}
	else
	{
		// 同步执行
		if( m_pDatabase->RealDirectExecute( sqlStr.c_str(), sqlStr.length() ) != true )
		{
			TRACE_ERROR( "Error: [%s][%d][%s], direct exec sql %s faild.\n", __MY_FILE__, __LINE__, __FUNCTION__, sqlStr.c_str() );
		}
	}

	return 0;
}