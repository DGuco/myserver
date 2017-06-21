//
// Created by DGuco on 17-3-1.
//
#include "../inc/gameserver.h"
#include "../logicmodule/inc/modulemanager.h"

template<> CGameServer* CSingleton<CGameServer>::spSingleton = NULL;

CGameServer::CGameServer()
{
    miServerState = 0;
    mLastTickCount = 0;
}

CGameServer::~CGameServer()
{
    if (mpClientHandle != NULL)
    {
        delete mpClientHandle;
    }

    if (mpMessageDispatcher != NULL)
    {
        delete mpMessageDispatcher;
    }

    if (mpMessageFactory != NULL)
    {
        delete mpMessageFactory;
    }

    if (mpModuleManager != NULL)
    {
        delete mpModuleManager;
    }

    if (mpTimerManager != NULL)
    {
        delete mpTimerManager;
    }
}

int CGameServer::Initialize()
{
    mpModuleManager = new CModuleManager;
}

