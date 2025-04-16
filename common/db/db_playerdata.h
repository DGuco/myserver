//
// Created by DGuco on 17-9-6.
// 玩家基础信息
//

#ifndef DB_PLAYERDATA_H
#define DB_PLAYERDATA_H

#include "db_playerbase.h"
#include "db_playercity.h"
#include "shm_pool.h"

struct DBPlayerData 
{
    DBPlayerBase m_stPlayerBase; //基础数据
    DBPlayerCity m_stPlayerCity; //城建数据

    int Save(CSafePtr<IDataBase> pDataBase)
    {
        int bRet = m_stPlayerBase.Save(pDataBase);
        bRet = m_stPlayerCity.Save(pDataBase);
        return bRet;
    }
};

class CPlayerDataShmPool : public CShmPool<DBPlayerData,MAX_PLAYER_POOL_SIZE>
{
public:
    CPlayerDataShmPool() {};
    virtual ~CPlayerDataShmPool() {};
    virtual enShmType GetShmType() {return enShmType_Player;};
};


#endif //DB_PLAYERBASE_H
