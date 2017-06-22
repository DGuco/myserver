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

void CCoreModule::OnClientMessage(CTeam *pTeam, CMessage *pMsg)
{

}

int CCoreModule::OnCreateEntity(CTeam *pTeam)
{
    return 0;
}

void CCoreModule::OnDestroyEntity(CTeam *pTeam)
{

}