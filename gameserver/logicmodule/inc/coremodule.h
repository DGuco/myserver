//
//  coremodule.h
//  coremodule.h类头文件
//  Created by DGuco on 17-3-1.
//  Copyright © 2017年 DGuco. All rights reserved.
//
#ifndef SERVER_COREMODULE_H
#define SERVER_COREMODULE_H

#include "logicmodule.h"
#include "../../../framework/base/servertool.h"

class CCoreModule : public CLogicModule,public CSingleton<CCoreModule>
{
public:
    CCoreModule();
    virtual  ~CCoreModule();
    int Initialize();
    // 启动服务
    int OnLaunchServer();
    // 退出服务
    int OnExitServer();
    // 路由消息
    void OnRouterMessage(CMessage* pMsg);
    // 客户端消息
    void OnClientMessage(CPlayer* pTeam, CMessage* pMsg);
    // 创建实体
    int OnCreateEntity(CPlayer* pTeam);
    // 销毁实体
    void OnDestroyEntity(CPlayer* pTeam);
};

#endif //SERVER_COREMODULE_H
