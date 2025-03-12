//
// Created by dguco on 17-9-8.
//
#ifndef DB_PLAYERBASE_H
#define DB_PLAYERBASE_H

#include "common_def.h"
#include "base.h"

struct DBPlayerBase
{
    // 上次登录时间
    int m_iLastLoginTime;
    // 离线时间
    int m_iLeaveTime;
    // 创建时间
    time_t m_tCreateTime;
    // 帐号状态时长( 禁止登陆 禁止说话 禁止...)
    time_t m_tLoginLimitTime;
    // 帐号
    char	m_acAccount[UID_LENGTH];

    int Save(CSafePtr<IDataBase> pDataBase)
    {
        return 0;
    }
};


#endif //DB_PLAYERBASE_H
