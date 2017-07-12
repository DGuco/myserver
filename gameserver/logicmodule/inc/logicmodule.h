//
// Created by DGuco on 17-6-21.
// 模块接口
//

#ifndef SERVER_LOGIC_MODULE_H
#define SERVER_LOGIC_MODULE_H

// 模块类型
enum EModuleType
{
    EMODULETYPE_INVALID			= -1,	// 无效的模块

    EMODULETYPE_CORE			= 0,	// 核心模块
    EMODULETYPE_NUM						// 模块数量
};

#define EMODULETYPE_START (EMODULETYPE_INVALID + 1)	// 模块起始下标

class CTeam;
class CMessage;

class CLogicModule
{
public:
    CLogicModule();
    virtual ~CLogicModule();

    bool IsRegist();
    void RegistModule(EModuleType eType);

    virtual int Initialize() {return 0;}

public:
    bool 		mRegist;		// 是否注册
    EModuleType mModueType;		// 模块类型

public:
    // 启动服务
    virtual int OnLaunchServer() {return 0;}

    // 退出服务
    virtual int OnExitServer() {return 0;}

    // 路由消息
    virtual void OnRouterMessage(CMessage* pMsg) {}

    // 客户端消息
    virtual void OnClientMessage(CTeam* pTeam, CMessage* pMsg) {}

    // 创建实体
    virtual int OnCreateEntity(CTeam* pTeam) {return 0;}

    // 销毁实体
    virtual void OnDestroyEntity(CTeam* pTeam) {}
};

#endif //SERVER_LOGIC_MODULE_H
