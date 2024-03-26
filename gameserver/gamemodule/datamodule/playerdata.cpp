//
// Created by dguco on 17-9-6.
//

#include "player.h"

CPlayerData::CPlayerData(CPlayer* pPlayer) : m_pPlayer(pPlayer)
{

}

CPlayerData::~CPlayerData() {
    //���ﲻ�ͷ�ָ��ָ��Ŀռ䣬Ϊ�˷����õ�����ģ����������ﱣ����һ��ָ��������ݵ�
    //ָ�룬����������������ݵ�ʱ���ͷ�=>CSceneObjManager::DestroyPlayer
    m_pPlayer = NULL;
}

const CPlayer* CPlayerData::GetPlayer()
{
    if (m_pPlayer == NULL) {
        char msg[128];
        sprintf(msg, "[%s : %d : %s] Get player data error.,player id = %d",
                __MY_FILE__, __LINE__, __FUNCTION__);
        throw std::logic_error(msg);
    }
    return m_pPlayer;
}

OBJ_ID CPlayerData::GetPlayerId()
{
    return GetPlayer()->get_id();
}
