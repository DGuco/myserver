//
// Created by dguco on 17-9-12.
//

#ifndef SERVER_DBMODULE_H
#define SERVER_DBMODULE_H

#include "../../datamodule/inc/player.h"
#include "../../../framework/message/message_interface.h"
#include "logicmodule.h"
#include "../../../framework/timer/timer.h"
#include "../../../framework/message/dbmessage.pb.h"
#include "../../../framework/const/dblogintype.h"
#include "../../../framework/message/message.pb.h"

class CDbModule : public CLogicModule, public CSingleton<CDbModule> {
public:
    CDbModule();

    virtual  ~CDbModule();

    int Initialize();

    // 启动服务
    int OnLaunchServer();

    // 退出服务
    int OnExitServer();

    // 路由消息
    void OnRouterMessage(CProxyMessage *pMsg);

    // 客户端消息
    void OnClientMessage(CPlayer *pTeam,CMessage *pMsg);

    // 创建实体
    int OnCreateEntity(CPlayer *pTeam);

    // 销毁实体
    void OnDestroyEntity(CPlayer *pTeam);

private:
    int ExecuteSql(emDBLogicType nLogicType,
                   unsigned long ulTeamID,
                   int iSessionID,
                   long nTimeStamp,
                   SQLTYPE nSqlType,
                   int nProduOutNumber,
                   CALLBACK nIsCallBack,
                   const char* pSql,
                   CSocketInfo* pSocketInfo, ... );

    int ExecuteSqlForBlob(emDBLogicType nLogicType,
                          unsigned long ulTeamID,
                          int iSessionID, long nTimeStamp,
                          SQLTYPE nSqlType,
                          int nProduOutNumber,
                          CALLBACK nIsCallBack,
                          const char* pSql,
                          const int iBlobSize,
                          const char* pBlob,
                          const char* pSQLWhere,
                          CSocketInfo* pSocketInfo, ... );
public:
    //查找玩家帐号信息没有则创建
    void FindOrCreateUserRequest(std::string& platform,std::string& puid,CSocketInfo* pSocketInfo);
    void FindOrCreateUserResponse(CSession* pSession, CMsgExecuteSqlResponse* pMsgSql,CSocketInfo* pSocketInfo);
};

#endif //SERVER_DBMODULE_H
