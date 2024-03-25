//
// Created by DGuco on 17-9-6.
//

#include "playerbase.h"

CPlayerBase::CPlayerBase(CPlayer* pPlayer) : CPlayerData(pPlayer)
{
    Initialize();
}

CPlayerBase::~CPlayerBase()
{

}

int CPlayerBase::Initialize()
{
    return 0;
}