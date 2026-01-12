#include "homeplant_manager.h"

HomePlantManager* g_HomePlantManager = NULL;

SafePointer<Home> HomePlantManager::GetHome(GUID64_t ownerID)
{
    __ENTER_FUNCTION
    std::map<GUID64_t, Home*>::iterator it = m_HomeMap.find(ownerID);
    if(it == m_HomeMap.end())
    {
        return NULL;
    }
    return it->second;
    __LEAVE_FUNCTION
    return NULL;
}

VOID HomePlantManager::UnlockTrunk(Obj_Human* pOwner, SHORT nTrunkID)
{
    __ENTER_FUNCTION
    if(pOwner == NULL || pOwner->getScene() == NULL || pOwner->getScene()->GetLuaInterface() == NULL)
    {
        return;
    }

    if(nTrunkID < 0 || nTrunkID >= MAX_HOME_TRANK)
    {
        return;
    }

    SafePointer<Home> pHome = GetHome(pOwner->GetGUID());
    if(pHome == NULL)
    {
        return;
    }

    LuaInterface* pLuaInterface = pOwner->getScene()->GetLuaInterface();
    Scene* pScene = pOwner->getScene();
    Assert(pLuaInterface != NULL && pScene != NULL);
    pHome->m_HomePlant.m_TrunkArray[nTrunkID].m_bState = 1;
    pLuaInterface->ExeScript_DD(HONE_PLANT_SCEIPT_ID,"OnPlantTrunkUnlock", pScene->SceneID(), nTrunkID);
    __LEAVE_FUNCTION
}

VOID HomePlantManager::WateringTrunk(Obj_Human* pOwner, SHORT nTrunkID)
{
    __ENTER_FUNCTION
    if(pOwner == NULL || pOwner->getScene() == NULL || pOwner->getScene()->GetLuaInterface() == NULL)
    {
        return;
    }

    if(nTrunkID < 0 || nTrunkID >= MAX_HOME_TRANK)
    {
        return;
    }

    SafePointer<Home> pHome = GetHome(pOwner->GetGUID());
    if(pHome == NULL)
    {
        return;
    }

    // 检查地块儿是否已解锁
    if(pHome->m_HomePlant.m_TrunkArray[nTrunkID].m_bState != 1)
    {
        return;
    }
    
    LuaInterface* pLuaInterface = pOwner->getScene()->GetLuaInterface();
    Scene* pScene = pOwner->getScene();
    Assert(pLuaInterface != NULL && pScene != NULL);
    pHome->m_HomePlant.m_TrunkArray[nTrunkID].m_bState = 2;
    pLuaInterface->ExeScript_DD(HONE_PLANT_SCEIPT_ID,"OnPlantTrunkWatering", pScene->SceneID(), nTrunkID);
    __LEAVE_FUNCTION
}

// 填充家园信息
VOID HomePlantManager::FillHomePlatMsg(WGHomePlantOpt* pMsg, GUID64_t ownerID)
{
    __ENTER_FUNCTION
    if(pMsg == NULL)
    {
        return;
    }
    SafePointer<Home> pHome = GetHome(ownerID);
    if(pHome == NULL)
    {
        return;
    }
    pMsg->SetHomePlant(pHome->m_HomePlant);
    __LEAVE_FUNCTION
}
