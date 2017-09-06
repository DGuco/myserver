//
// Created by dguco on 17-9-6.
//

#include "../inc/player.h"

CPlayerData::CPlayerData(std::shared_ptr <CPlayer> pPlayer)
{

}

const std::shared_ptr<CPlayer> CPlayerData::GetPlayer()
{
    std::shared_ptr player = m_wpPlayer.lock();
    if (player == NULL) {
        char msg[128];
        sprintf(msg, "[%s : %d : %s] Get player data error.,player id = %d",
                __MY_FILE__, __LINE__, __FUNCTION__);
        throw std::logic_error(msg);
    }
    return m_wpPlayer.lock();
}

OBJ_ID CPlayerData::GetPlayerId()
{
    return GetPlayer()->get_id();
}
