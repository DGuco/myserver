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


void CCoreModule::OnRouterMessage(CMessage *pMsg)
{

}

void CCoreModule::OnClientMessage(CPlayer *pTeam, CMessage *pMsg)
{

}

int CCoreModule::OnCreateEntity(CPlayer *pTeam)
{
    return 0;
}

void CCoreModule::OnDestroyEntity(CPlayer *pTeam)
{

}