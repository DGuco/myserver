#include "../inc/coremodule.h"

CCoreModule::CCoreModule()
{

}

CCoreModule::~CCoreModule()
{

}

int CCoreModule::Initialize()
{
    return 0;
}

int CCoreModule::OnLaunchServer()
{
    return 0;
}

int CCoreModule::OnExitServer()
{
    return 0;
}


void CCoreModule::OnRouterMessage(int cmd,Message *pMsg)
{

}

void CCoreModule::OnClientMessage(CPlayer *pTeam, int cmd,Message *pMsg)
{

}

int CCoreModule::OnCreateEntity(CPlayer *pTeam)
{
    return 0;
}

void CCoreModule::OnDestroyEntity(CPlayer *pTeam)
{

}