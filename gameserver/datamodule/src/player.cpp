//
// Created by dguco on 17-7-23.
//
#include "../inc/player.h"

CPlayer::CPlayer()
{
    Initialize();
}

CPlayer::~CPlayer()
{
}

int CPlayer::Initialize()
{
    m_oPackage.Initialize();
    m_spPlayerBase = std::make_shared(CPlayerBase(this));
    m_spPlayerCity = std::make_shared(CPlayerCity(this));
    return 0;
}