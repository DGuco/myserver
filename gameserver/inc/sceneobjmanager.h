//
// Created by DGuco on 17-7-27.
//

#ifndef SERVER_SCENEOBJMANAGER_H
#define SERVER_SCENEOBJMANAGER_H

#include <cstddef>
#include <unordered_map>
#include "../../framework/base/servertool.h"
#include "../../framework/base/base.h"
#include "../datamodule/inc/player.h"

#ifdef _DEBUG_
#define SERVER_CAP_TEAM				(500)		// 服务器最大战队实体数量
#define SERVER_TEAM_LIST			(5000)		// 服务器最大注册列表
#define SERVER_CAP_ONLINE			(SERVER_CAP_TEAM) // 服务器最大链接数量
#define SERVER_LOGIN_ONLINE			(400)		// 玩家链接最大限制
#define SERVER_LOGIN_MAX			(450)		// 登陆玩家数量删除标准
#define SERVER_CAP_MAIL				(500)		// 服务器最大邮件数量
#else
#define SERVER_CAP_TEAM				(6000)			// 服务器最大战队实体数量
#define SERVER_TEAM_LIST			(500000)		// 服务器最大注册列表
#define SERVER_CAP_ONLINE			(SERVER_CAP_TEAM) // 服务器最大链接数量
#define SERVER_LOGIN_ONLINE			(5000)			// 玩家链接最大限制
#define SERVER_LOGIN_MAX			(5500)			// 登陆玩家数量删除标准
#define SERVER_CAP_MAIL 			(100000)		// 服务器最大邮件数量
#endif

// 最大服务器数量
#define MAX_SERVER_NUM		(9999)

// 服务器timer数量
#define SERVER_CAP_TIMER		(20000)

// 服务器session数量
#define SERVER_CAP_SESSION		(20000)

class CObj;

class CSceneObjManager : public CSingleton<CSceneObjManager>
{
public:
    CSceneObjManager();
    ~CSceneObjManager();

    int Initialize();

public:
    // 删除对象,在不知道对象类型的情况下使用
    int DestroyObject(OBJ_ID iObjID);
    // 获得对象，在不知道对象类型的情况下使用
    CObj* GetObject(OBJ_ID iObjID);
    // 获取玩家
    CPlayer* GetPlayer(OBJ_ID ulPlayerid);
    //删除玩家
    int DestroyPlayer(OBJ_ID iObjID);
    //添加玩家
    void AddNewPlayer(CPlayer *pPlayer);

private:
    std::unordered_map<uint64,CPlayer*> m_mPlayerMap;
};


#endif //SERVER_SCENEOBJMANAGER_H
