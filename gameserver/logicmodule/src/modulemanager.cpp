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
    pTmpCoreModule->RegistModule(EModuleType::EMODULETYPE_CORE);
    mpLogicModules[EModuleType::EMODULETYPE_CORE] = pTmpCoreModule;

}