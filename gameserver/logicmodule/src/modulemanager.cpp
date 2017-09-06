//
// Created by DGuco on 17-6-21.
//
#include "../inc/modulemanager.h"
#include "../inc/coremodule.h"

template<> CModuleManager* CSingleton<CModuleManager>::spSingleton = NULL;

CModuleManager::CModuleManager()
{
}

CModuleManager::~CModuleManager()
{
}

int CModuleManager::Initialize()
{
    for (auto i = 0; i < EMODULETYPE_NUM; i++)
    {
        mpLogicModules[i] = NULL;
    }
    //游戏主模块
    CCoreModule* pTmpCoreModule = new CCoreModule;
    RegisterModule(EModuleType::EMODULETYPE_CORE,pTmpCoreModule);
}

int CModuleManager::RegisterModule(EModuleType eType, CLogicModule *pModule)
{
    if (pModule == NULL)
    {
        LOG_FATAL("default", "[%s : %d : %s] RegisterModule failed, eType(%d) pModule == NULL.",
                  __MY_FILE__, __LINE__, __FUNCTION__, eType);
        return -1;
    }

    if (eType <= EModuleType::EMODULETYPE_INVALID || eType >= EModuleType::EMODULETYPE_NUM)
    {
        LOG_FATAL("default", "[%s : %d : %s] RegisterModule failed, eType(%d)",
                  __MY_FILE__, __LINE__, __FUNCTION__, eType);
        return -2;
    }

    mpLogicModules[eType] = pModule;
    pModule->RegistModule(eType);
    return 0;
}

int CModuleManager::OnLaunchServer()
{
    int iRet = 0;
    for (int i = EMODULETYPE_START;i < EModuleType::EMODULETYPE_NUM;++i)
    {
        iRet = mpLogicModules[i]->OnLaunchServer();
    }
    return iRet;
}

int CModuleManager::OnExitServer()
{
    int iRet = 0;
    for (int i = EMODULETYPE_START;i < EModuleType::EMODULETYPE_NUM;++i)
    {
        iRet = mpLogicModules[i]->OnExitServer();
    }
    return iRet;
}

void CModuleManager::OnRouterMessage(int iModuleType, CMessage *pMsg)
{
    if (iModuleType < EMODULETYPE_START || iModuleType >= EMODULETYPE_NUM)
    {
        return;
    }

    mpLogicModules[iModuleType]->OnRouterMessage(pMsg);
}

void CModuleManager::OnClientMessage(int iModuleType, CPlayer *pTeam, CMessage *pMsg)
{
    if (iModuleType < EMODULETYPE_START || iModuleType >= EMODULETYPE_NUM)
    {
        return;
    }

    mpLogicModules[iModuleType]->OnClientMessage(pTeam, pMsg);
}

int CModuleManager::OnCreateEntity(CPlayer *pTeam)
{
    int iRet = 0;
    for (int i = EMODULETYPE_START; i < EMODULETYPE_NUM; i++)
    {
        iRet = mpLogicModules[i]->OnCreateEntity(pTeam);
        if (iRet != 0)
        {
            return iRet;
        }
    }
    return iRet;
}

void CModuleManager::OnDestroyEntity(CPlayer *pTeam)
{
    for (int i = EMODULETYPE_START; i < EMODULETYPE_NUM; i++)
    {
        mpLogicModules[i]->OnDestroyEntity(pTeam);
    }
}