//
// Created by DGuco on 17-7-27.
//

#include "../inc/sceneobjmanager.h"

template<> CSceneObjManager* CSingleton<CSceneObjManager>::spSingleton = NULL;


CSceneObjManager::CSceneObjManager()
{
    Initialize();
}


CSceneObjManager::~CSceneObjManager()
{
    if (m_pPlayerManager != NULL)
    {
        delete m_pPlayerManager;
        m_pPlayerManager = NULL;
    }

}

int CSceneObjManager::Initialize()
{
    m_pPlayerManager = new CObjectManager(EnObjType::OBJ_PLAYER_ENTRY);
    m_mPlayerMap.clear();
    return 0;
}

// 删除对象
int CSceneObjManager::DestroyObject(OBJ_ID iObjID)
{
    EnObjType eType = (EnObjType) CObj::ID2TYPE(iObjID);

    switch(eType)
    {
        case EnObjType::OBJ_PLAYER_ENTRY :
        {
            auto it = m_mPlayerMap.find(iObjID);
            if (it != m_mPlayerMap.end()) {
                delete it->second;
            }
            m_mPlayerMap.erase(iObjID);
            return 0;
        }
        default:
        {
            LOG_ERROR("default", "[%s : %d : %s] DestroyObject failed, object id = %d, type = %d.",
                      __MY_FILE__, __LINE__, __FUNCTION__, iObjID, eType);
            return -1;
        }
    }
}

// 获得对象
CObj* CSceneObjManager::GetObject(OBJ_ID iObjID)
{
    if (iObjID == INVALID_OBJ_ID)
    {
        return NULL;
    }

    EnObjType tType = (EnObjType) CObj::ID2TYPE(iObjID);

    switch(tType)
    {
        case EnObjType::OBJ_PLAYER_ENTRY :
        {
            auto it = m_mPlayerMap.find(iObjID);
            if (it != m_mPlayerMap.end()) {
                return it->second;
            }
            return NULL;
        }
        default:
        {
            LOG_ERROR("default", "[%s : %d : %s] get object failed, object id(%d), type(%d), not registed.",
                      __MY_FILE__, __LINE__, __FUNCTION__, iObjID, tType);
            return NULL;
        }
    }
}


CPlayer* CSceneObjManager::GetPlayer(OBJ_ID ulPlayerid)
{
    auto it = m_mPlayerMap.find(ulPlayerid);
    if (it != m_mPlayerMap.end()) {
        return it->second;
    }
    return NULL;
}

//删除玩家
int CSceneObjManager::DestroyPlayer(OBJ_ID iObjID)
{
    auto it = m_mPlayerMap.find(iObjID);
    if (it != m_mPlayerMap.end()) {
        delete it->second;
    }
    m_mPlayerMap.erase(iObjID);
}

void CSceneObjManager::AddNewPlayer(CPlayer *pPlayer)
{
    if (pPlayer == NULL) {
        return;
    }
    m_mPlayerMap[pPlayer->GetPlayerId()] = pPlayer;
}

CObjectManager* CSceneObjManager::GetPlayerManager()
{
    return m_pPlayerManager;
}
