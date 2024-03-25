//
// Created by dguco on 17-2-16.
//
#include "safe_pointer.h"
#include "server_config.h"

CServerConfig::CServerConfig()
{
    Clear();
}

CServerConfig::~CServerConfig()
{
    Clear();
}

int CServerConfig::Parse()
{
    m_iTcpKeepAlive = m_Obj["keepalive"].GetInt();
    m_iServerTick = m_Obj["servertick"].GetInt();
    m_iChecktimeOutGap = m_Obj["checktimeOutGap"].GetInt();
    m_iSokcetTimeout = m_Obj["socketTimeout"].GetInt();

    ServerInfo gameServer;
    Value& gameinfo = m_Obj["gameinfo"];
    gameServer.m_iServerId =  gameinfo["serverid"].GetInt();
    gameServer.m_iPort =  gameinfo["port"].GetInt();
    gameServer.m_sHost =  gameinfo["host"].GetString();
    m_mServerMap[enServerType::FE_GAMESERVER] = gameServer;

    ServerInfo proxyServer;
    Value& proxyinfo = m_Obj["proxyinfo"];
    proxyServer.m_iServerId =  proxyinfo["serverid"].GetInt();
    proxyServer.m_iPort =  proxyinfo["port"].GetInt();
    proxyServer.m_sHost =  proxyinfo["host"].GetString();
    m_mServerMap[enServerType::FE_PROXYSERVER] = proxyServer;

    m_sDblInfo = m_Obj["mysqlinfo"].GetString();
    m_iDbSleepTime = m_Obj["sleeptime"].GetInt();
    m_iDbLoop = m_Obj["loop"].GetInt();

    return 0;
}

void CServerConfig::Clear()
{
    m_iSokcetTimeout = 0;
    m_iChecktimeOutGap = 0;
    m_iTcpKeepAlive = 0;
    m_iServerTick = 0;
    m_mServerMap.clear();
}


CSafePtr<ServerInfo> CServerConfig::GetServerInfo(enServerType type)
{
    auto it =  m_mServerMap.find(type);
    if (it != m_mServerMap.end())
    {
        return &(it->second);
    }
    return NULL;
}
