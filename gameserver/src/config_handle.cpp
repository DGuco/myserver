//
// Created by dguco on 18-9-3.
//

#include <config.h>
#include "../inc/config_handle.h"

int CConfigHandle::PrepareToRun()
{
	LoadFile("serverinfo.json");
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
	m_mConfigMap.insert(std::make_pair(fileName, pTmpConfig));
}
