//
// Created by DGuco on 17-7-23.
//

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <string.h>
#include "playerbase.h"
#include "playercity.h"
#include "base.h"
#include "object.h"

class CPlayerBase;
class CPlayerCity;

class CPlayer : public CObj
{
public:
    CPlayer(OBJ_ID playerId);
    virtual ~CPlayer();
    virtual int Initialize();

public:
    CPlayerBase* GetPlayerBase() {return m_spPlayerBase;}
    CPlayerCity* GetPlayerCity() {return m_spPlayerCity;}
    const OBJ_ID GetPlayerId() {return get_id();}
private:
    CPlayerBase* m_spPlayerBase;    //玩家基础信息
    CPlayerCity* m_spPlayerCity;    //玩家城池信息
};

#endif //SERVER_PLAYER_H_H
