//
// Created by DGuco on 17-9-6.
//

#ifndef SERVER_PLAYERDATA_H
#define SERVER_PLAYERDATA_H

#include <memory>
#include "player.h"
#include "../../../framework/base/object.h"

class CPlayer;
class CPlayerData
{
public:
    CPlayerData(std::shared_ptr<CPlayer> pPlayer) : m_wpPlayer(pPlayer){}

private:
    std::weak_ptr<CPlayer> m_wpPlayer;
public:
    const std::shared_ptr<CPlayer> GetPlayer();
    OBJ_ID GetPlayerId();
};

#endif //SERVER_PLAYERDATA_H
