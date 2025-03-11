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
#include "shm_def.h"

struct DBPlayerData 
{
    DBPlayerBase m_stPlayerBase; //基础数据
    DBPlayerCity m_stPlayerCity; //城建数据
};

class CPlayerBaseShmPool : public CShmPool<DBPlayerData,MAX_PLAYER_POOL_SIZE,MAX_PLAYER_SAVING_SIZE>
{
public:
    CPlayerBaseShmPool() {};
    virtual ~CPlayerBaseShmPool() {};
    virtual enShmType GetShmType() {return enShmType_Player;};
}；


#endif //DB_PLAYERBASE_H
