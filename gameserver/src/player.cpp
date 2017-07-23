//
// Created by dguco on 17-7-23.
//

#ifndef TEAM_H_
#define TEAM_H_

#include "../inc/player.h"

CPlayer::CPlayer()
{
    Initialize();
}


int CPlayer::Initialize()
{
    miLastLoginTime	= 0;
    miLeaveTime		= 0;
    mtCreateTime			= 0;
    mtLoginLimitTime		= 0;
    memset(macTeamName, 0, ARRAY_CNT(macTeamName));
    memset(macAccount, 0, ARRAY_CNT(macAccount));
    return 0;
}

#endif