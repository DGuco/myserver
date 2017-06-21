//
//  module_manager.h
//  模块管理 头文件
//  Created by DGuco on 17-06-21.
//  Copyright © 2017年 DGuco. All rights reserved.
//

#ifndef SERVER_MODULE_MANAGER_H
#define SERVER_MODULE_MANAGER_H

#include "../../../framework/base/servertool.h"
#include "logicmodule.h"

class CLogicModule;

class CModuleManager : public CSingleton<CModuleManager>
{
public:
    CModuleManager();
    ~CModuleManager();

    int Initialize();

    // 注册模块
    int RegisterModule(EModuleType eType, CLogicModule* pModule);

    // 启动服务
    int OnLaunchServer();

    // 退出服务
    int OnExitServer();

    // 路由消息
    void OnRouterMessage(int iModuleType, CMessage* pMsg);

    // 客户端消息
    void OnClientMessage(int iModuleType, CTeam* pTeam, CMessage* pMsg);

    // 创建实体
    int OnCreateEntity(CTeam* pTeam);

    // 销毁实体
    void OnDestroyEntity(CTeam* pTeam);

protected:
    CLogicModule* mpLogicModules[EMODULETYPE_NUM];
};

#endif //SERVER_MODULE_MANAGER_H
