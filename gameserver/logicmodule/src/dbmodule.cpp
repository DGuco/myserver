//
// Created by dguco on 17-9-12.
//

#include "../inc/dbmodule.h"
#include "../../inc/gameserver.h"
#include "../../../framework/const/dblogintype.h"
#include "../../../framework/message/message.pb.h"
#include "../../../framework/message/server_comm_engine.h"
#include "../../../framework/base/servertool.h"

template<> CDbModule* CSingleton<CDbModule>::spSingleton = NULL;

CDbModule::CDbModule() {

}

CDbModule::~CDbModule() {

}

int CDbModule::OnLaunchServer()
{

}

int CDbModule::OnExitServer()
{

}

void CDbModule::OnRouterMessage(CProxyMessage *pMsg)
{

}

void CDbModule::OnClientMessage(CPlayer *pTeam,CMessage *pMsg)
{

}

int CDbModule::OnCreateEntity(CPlayer *pTeam)
{

}

void CDbModule::OnDestroyEntity(CPlayer *pTeam)
{

}


int CDbModule::ExecuteSql(emDBLogicType nLogicType,
                          unsigned long ulTeamID,
                          int iSessionID,
                          long nTimeStamp,
                          SQLTYPE nSqlType,
                          int nProduOutNumber,
                          CALLBACK nIsCallBack,
                          const char* pSql,
                          CSocketInfo* pSocketInfo, ... )
{
    MY_ASSERT_LOG("db", pSql != NULL, return -1);

    // 参数整理
    va_list tmpArgs;
    char acTmpSqlCommond[MAX_PACKAGE_LEN] = {0};
    va_start( tmpArgs, pSql);
    int iTmpLen = ::vsnprintf( acTmpSqlCommond, sizeof(acTmpSqlCommond), pSql, tmpArgs );
    MY_ASSERT_LOG("db", iTmpLen > -1, return -2);
    va_end(tmpArgs);

    // 发往dbserver消息整理
    static CProxyMessage tmpMsg;
    static CMsgExecuteSqlRequest tmpMsgSqlRqt;
    tmpMsg.Clear();
    tmpMsgSqlRqt.Clear();

    CProxyHead* pTmpHead = tmpMsg.mutable_msghead();
    pbmsg_setmessagehead(pTmpHead, CMsgExecuteSqlRequest::MsgID );
    if (pSocketInfo != NULL)
    {
        CSocketInfo* socketInfo = pTmpHead->mutable_socketinfo();
        socketInfo->set_socketid(pSocketInfo->socketid());
        socketInfo->set_createtime(pSocketInfo->createtime());
    }

    tmpMsgSqlRqt.set_logictype(nLogicType);
    tmpMsgSqlRqt.set_sessionid(iSessionID);
    tmpMsgSqlRqt.set_timestamp(nTimeStamp);
    tmpMsgSqlRqt.set_teamid(ulTeamID);
    tmpMsgSqlRqt.set_sqltype(nSqlType);
    tmpMsgSqlRqt.set_outnumber(nProduOutNumber);
    tmpMsgSqlRqt.set_callback(nIsCallBack);
    tmpMsgSqlRqt.set_sql(acTmpSqlCommond);
    tmpMsgSqlRqt.set_hasblob(NONEBLOB);
    tmpMsg.set_msgpara((unsigned long)&tmpMsgSqlRqt);

	LOG_DEBUG("db", "[%s]", ((Message*)tmpMsg.msgpara())->ShortDebugString().c_str());

    MY_ASSERT_LOG("db", CGameServer::GetSingletonPtr()->SendMessageToDB(&tmpMsg), return -3);

    return 0;
}

int CDbModule::ExecuteSqlForBlob(emDBLogicType nLogicType,
                                 unsigned long ulTeamID,
                                 int iSessionID,
                                 long nTimeStamp,
                                 SQLTYPE nSqlType,
                                 int nProduOutNumber,
                                 CALLBACK nIsCallBack,
                                 const char* pSql,
                                 const int iBlobSize,
                                 const char* pBlob,
                                 const char* pSQLWhere,
                                 CSocketInfo* pSocketInfo,... )
{
    MY_ASSERT_LOG("db", pSql != NULL && pBlob != NULL && pSQLWhere != NULL, return -1);

    // 发往dbserver消息整理
    static CProxyMessage tmpMsg;
    static CMsgExecuteSqlRequest tmpMsgSqlRqt;
    tmpMsg.Clear();
    tmpMsgSqlRqt.Clear();

    CProxyHead* pTmpHead = tmpMsg.mutable_msghead();
    pbmsg_setmessagehead(pTmpHead, CMsgExecuteSqlRequest::MsgID );
    if (pSocketInfo != NULL)
    {
        CSocketInfo* socketInfo = pTmpHead->mutable_socketinfo();
        socketInfo->set_socketid(pSocketInfo->socketid());
        socketInfo->set_createtime(pSocketInfo->createtime());
    }

    tmpMsgSqlRqt.set_logictype(nLogicType);
    tmpMsgSqlRqt.set_sessionid(iSessionID);
    tmpMsgSqlRqt.set_timestamp(nTimeStamp);
    tmpMsgSqlRqt.set_teamid(ulTeamID);
    tmpMsgSqlRqt.set_sqltype(nSqlType);
    tmpMsgSqlRqt.set_outnumber(nProduOutNumber);
    tmpMsgSqlRqt.set_callback(nIsCallBack);
    tmpMsgSqlRqt.set_sql(pSql);

    tmpMsgSqlRqt.set_bufsize( iBlobSize );
    tmpMsgSqlRqt.set_buffer( (void*)pBlob, iBlobSize );
    tmpMsgSqlRqt.set_sqlwhere( pSQLWhere );
    tmpMsgSqlRqt.set_hasblob(HASBLOB);

    tmpMsg.set_msgpara((unsigned long)&tmpMsgSqlRqt);
	LOG_DEBUG("db", "[%s]", ((Message*)tmpMsg.msgpara())->ShortDebugString().c_str());
    MY_ASSERT_LOG("db", CGameServer::GetSingletonPtr()->SendMessageToDB(&tmpMsg ), return -2);
    return 0;
}

void CDbModule::FindOrCreateUserRequest(std::string &platform, std::string &puid,CSocketInfo* pSocketInfo)
{
    char* pcTmpSql = (char*)"SELECT `player_id` FROM user WHERE `platform` = %s AND `puid` = %s";
    int iRet = CDbModule::GetSingletonPtr()->ExecuteSql(
            emDBLogicType::emDBTypeFindOrCreateUser,
            0,
            0,
            0,
            SELECT,
            0,
            MUSTCALLBACK,
            pcTmpSql,
            pSocketInfo,
            platform,
            puid
    );
    if (iRet != 0)
    {
        LOG_ERROR("db", "[%s : %d : %s] ExecuteSql failed, iRet=%d.",
                  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
//        exit(0);
    }
}

void CDbModule::FindOrCreateUserResponse(CSession *pSession, CMsgExecuteSqlResponse *pMsgSql,CSocketInfo* pSocketInfo)
{
    MY_ASSERT_LOG("db",  pMsgSql != NULL && pSession != NULL && pSocketInfo != NULL, return);

    int iTmpRowCount = pMsgSql->rowcount();		// 行数
    int iTmpColCount = pMsgSql->colcount();		// 列数

    // 必须对行数列数判断
    if ( 1 == pMsgSql->resultcode() && iTmpRowCount > 0
         &&  emCol1 == iTmpColCount && emRow1 == iTmpRowCount )
    {
        unsigned long	player_id = 0;	// 帐号

        // 表示有返回结果
        for(int i = 0; i < iTmpRowCount ; i++)
        {
            player_id		= atoll((char*)pMsgSql->fieldvalue( 0 + i * iTmpColCount ).c_str());
        }
    }
    else
    {
        // 拉取数据失败,则退出
        LOG_ERROR("db", "[%s : %d : %s] LoadAllAccountResponse failed, resultcode=%d, row=%d, col=%d.",
                  __YQ_FILE__, __LINE__, __FUNCTION__, pMsgSql->resultcode(), iTmpRowCount, iTmpColCount);
        return;
    }
    LoadAllDataFinish(pTmpTeam, ELSF_ACCOUNTDATA);
}