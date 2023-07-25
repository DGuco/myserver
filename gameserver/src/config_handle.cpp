//
// Created by dguco on 18-9-3.
//

#include <config.h>
#include "../inc/game_server.h"
#include "../inc/config_handle.h"

CConfigHandle::CConfigHandle()
{
    m_pNetWork = CNetWork::GetSingletonPtr();
}

int CConfigHandle::PrepareToRun()
{
    bool bRet = m_pNetWork->ListenFile("../config", &CConfigHandle::lcb_OnConfigChanged, IN_MODIFY);
    if (!bRet) {
        LOG_ERROR("default", "Listen config failed");
        exit(0);
    }
    m_pServerConfig = CServerConfig::CreateInstance( );
    string filePath = "../config/serverinfo.json";
    if (-1 == m_pServerConfig->LoadFromFile(filePath)) {
        LOG_ERROR("default", "Get ServerConfig failed");
        exit(0);
    }
	return 0;
}

void CConfigHandle::Resume()
{
	PrepareToRun( );
}

void CConfigHandle::Reload(const string &fileName)
{
    auto it = m_mConfigMap.find(fileName);
    if (it != m_mConfigMap.end( )) {
        it->second->Reload( );
    }
}

void CConfigHandle::LoadFile(const string &fileName)
{
	shared_ptr<CServerConfig> &pTmpConfig = CServerConfig::CreateInstance( );
	if (-1 == pTmpConfig->LoadFromFile(configPath + fileName)) {
		LOG_ERROR("default", "Load file {} failed", fileName);
		exit(0);
	}
	m_mConfigMap.insert(std::make_pair(fileName, static_pointer_cast<AJson>(pTmpConfig)));
}

//void CConfigHandle::lcb_OnConfigChanged(inotify_event *notifyEvent)
//{
//
//    //todo 重新加载配置文件
//    char *fileName = notifyEvent->name;
//    CGameServer::GetSingletonPtr()->GetLogicThread()->PushTaskFront(
//            [fileName]()
//            {
//                CGameServer::GetSingletonPtr( )->GetConfigHandle( )->Reload(fileName);
//            });
//}