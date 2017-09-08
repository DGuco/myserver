//
// Created by DGuco on 17-9-6.
//

#include "../inc/playerbase.h"

CPlayerBase::CPlayerBase(std::shared_ptr<CPlayer> pPlayer) : CPlayerData(pPlayer)
{
    Initialize();
}

CPlayerBase::~CPlayerBase()
{

}

int CPlayerBase::Initialize()
{

}