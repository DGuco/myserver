//
// Created by DGuco on 17-7-7.
//

#include "objectManager.h"

CObjectManager::CObjectManager(EnObjType objType)
{
    m_enObjType = objType;
    m_tLastGetTime = GetSecondTime();
    m_ulLastUsedId = OBJ_ID_START(objType);
}

CObjectManager::~CObjectManager()
{
}

OBJ_ID CObjectManager::GetValidId()
{
    int timeNow = GetSecondTime();
    OBJ_ID id;

    if (m_ulLastUsedId > OBJ_ID_END(m_enObjType))
    {
        LOG_ERROR("default" ,"Id out of range,type = %d, id_start = %d, id_end = %d.",m_enObjType, OBJ_ID_START(m_enObjType), OBJ_ID_START(m_enObjType));
        return INVALID_OBJ_ID;
    }
    id = timeNow << 32 | m_ulLastUsedId;
    if (m_tLastGetTime == timeNow) {
        m_ulLastUsedId++;
    }else
    {
        m_tLastGetTime = timeNow;
        m_ulLastUsedId = OBJ_ID_START(m_enObjType);
    }
    return id;
}

void CObjectManager::SetLastUsedId(OBJ_ID ulLastUsedId)
{
    m_ulLastUsedId = ulLastUsedId;
}

void CObjectManager::SetLastGetTIme(time_t time)
{
    m_tLastGetTime = time;
}