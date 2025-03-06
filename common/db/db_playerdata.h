//
// Created by DGuco on 17-9-6.
// 玩家基础信息
//

#ifndef DB_PLAYERDATA_H
#define DB_PLAYERDATA_H

#include "base.h"
#include "db_playerbase.h"
#include "db_playercity.h"
#include "shm_pool.h"

struct DBPlayerData 
{
    DBPlayerBase m_stPlayerBase; //基础数据
    DBPlayerCity m_stPlayerCity; //城建数据
};

class PlayerBaseShmPool : public CShmPool<DBPlayerData,500,10>
{
public:
    PlayerBaseShmPool() {};
    virtual ~PlayerBaseShmPool() {};
    virtual enShmType GetShmType() {return enShmType_Player;};
}；


#endif //DB_PLAYERBASE_H
