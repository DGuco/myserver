//
// Created by dguco on 17-7-23.
//

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <string.h>
#include <ctime>
#include "../../framework/base/base.h"
#include "../../framework/base/object.h"
#include "../../framework/base/servertool.h"
#include "../../framework/base/my_macro.h"

// 玩家链接信息
struct STConnectInfo
{
    unsigned int		uiIP;													// IP
    unsigned short	unPort;											// 端口
    int							iSocket;											// socketid即FD
    time_t						tCreateTime;									// 创建时间
    time_t						tLastActiveTime;							// 最后活跃时间
    time_t						tLoginTime;									// 登陆时间
    char						acAddress[ADDR_LENGTH];		// 地址信息
    long						lMsgGuid;										// 消息唯一ID

    STConnectInfo()
    {
        Initialize();
    }

    int Initialize()
    {
        uiIP = 0;
        unPort = 0;
        iSocket = 0;
        tCreateTime = 0;
        tLastActiveTime = 0;
        tLoginTime = 0;
        lMsgGuid = 0;

        memset(acAddress, 0, sizeof(acAddress));

        return 0;
    }

    void SetConnectInfo(unsigned int uiIPPara, unsigned short unPortPara, int iSocketPara, time_t tCreateTimePara, time_t tLastActiveTimePara, time_t tLoginTimePara)
    {
        uiIP = uiIPPara;
        unPort = unPortPara;
        iSocket = iSocketPara;
        tCreateTime = tCreateTimePara;
        tLastActiveTime = tLastActiveTimePara;
        tLoginTime = tLoginTimePara;

        SockAddrToString(uiIP, unPort, acAddress);
    }

    void Clear()
    {
        uiIP = 0;
        unPort = 0;
        iSocket = 0;

        memset(acAddress, 0, sizeof(acAddress));
    }
};


class CPlayer : public CObj
{
public:
    enum ETeamState
    {
        ETS_INVALID								= 0,	// 无效的状态,也是初始状态
        ETS_STARTLOGIN						= 1,	// 刚创建,初始化了链接信息,其它信息为空
        ETS_LOADACCOUNT				= 2,	// 刚登陆,拉取帐号信息
        ETS_LOADDATA							= 3,	// 拉取战队数据
        ETS_INGAMECONNECT			= 4,	// 在游戏中,并且有连接
        ETS_INGAMEDISCONNECT		= 5,	// 在游戏中,无连接
        ETS_EXITSAVE							= 6,	// 离开存储数据
    };

    enum ESLF_FLAGS
    {
        ELSF_LOADALL	= 0x0,  // todo fix it
        ELSF_SAVEALL	= 0x1,
    };

    enum emFirstLoginFlag
    {
        emLoginFlag_First				= 0,		// 第一次登录
        emLoginFlag_notFirst		= 1,		// 不是地一次登录
        emLoginFlag_haveobj			= 2,		// 玩家存在实体
    };

    // 玩家离线类型
    // < 0 tcp主动断连或检测到连接断开
    // = 0 正常断连
    // > 0 服务器主动断连
    enum ELeaveType
    {
        LEAVE_CLIENTCLOSE		= -1,		// TCP检测到客户端断连,不需要回复
        LEAVE_TIMEOUT				= -2,		// TCP检测到客户端超时,不需要回复
        LEAVE_ERRPACKAGE			= -3,		// TCP检测到包信息非法,不需要回复
        LEAVE_BUFFOVER				= -4,		// TCP缓冲区已满,不需要回复
        LEAVE_PIPEERR					= -5,		// TCP数据包放入管道失败,不需要回复
        LEAVE_SYSTEM					= -6,		// TCP系统错误，暂时未用,不需要回复
        LEAVE_PUSHAGAINST		= -7,		// 被挤下去,不需要回复
        LEAVE_NOCONTRAL			= -8,		// 玩家长时间未操作,不需要回复
        LEAVE_NORMAL 				= 0,		// 玩家正常下线,需要回复
        LEAVE_LOGINFAILED		= 1,		// 玩家登陆失败,需要回复
    };

public:
    CPlayer();

    virtual int Initialize();
    virtual int Resume();

    // 实体ID
    int GetEntityID() {return get_id();}

protected:
    // 上次登录时间
    int miLastLoginTime;
    // 离线时间
    int miLeaveTime;
    // 战队ID
    unsigned long	mulPlayerID;
    // 创建时间
    time_t mtCreateTime;
    // 帐号状态时长( 禁止登陆 禁止说话 禁止...)
    time_t mtLoginLimitTime;
    // 战队名称
    char			macTeamName[NAME_LENGTH];
    // 帐号
    char			macAccount[UID_LENGTH];
    //连接信息
    STConnectInfo	mSocketInfo;
public:
    // 上次登录时间
    void SetLastLoginTime(int v) {miLastLoginTime = v;}
    int GetLastLoginTime() {return miLastLoginTime;}
    // 离线时间
    void SetLeaveTime(int v) {miLeaveTime = v;}
    int GetLeaveTime() {return miLeaveTime;}
    // 创建时间
    void SetCreateTime(time_t v) {mtCreateTime = v;}
    time_t GetCreateTime() {return mtCreateTime;}

    // 帐号
    void SetAccount(const char* p)
    {
        if (p == NULL) {return;}
        int iTmpLen = strlen(p);
        strncpy(macAccount, p, iTmpLen >= ARRAY_CNT(macAccount) ? (ARRAY_CNT(macAccount) - 1) : iTmpLen);
    }
    char* GetAccount() {return &macAccount[0];}
public:
    // 获取连接信息
    STConnectInfo* GetSocketInfoPtr() {return &mSocketInfo;}
};
#endif //SERVER_PLAYER_H_H
