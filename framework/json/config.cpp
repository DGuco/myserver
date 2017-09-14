//
// Created by dguco on 17-2-16.
//
#include "config.h"

template<> CServerConfig* CSingleton<CServerConfig>::spSingleton = NULL;
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

    ServerInfo tcpServer;
    Value& gateinfo = m_Obj["gateinfo"];
    tcpServer.m_iPort =  gateinfo["port"].GetInt();
    tcpServer.m_sHost =  gateinfo["host"].GetString();
    tcpServer.m_iServerId = gateinfo["serverid"].GetInt();
    m_mServerMap[enServerType::FE_GATESERVER] = tcpServer;

    ServerInfo gameServer;
    Value& gameinfo = m_Obj["gameinfo"];
    gameServer.m_iServerId =  gameinfo["serverid"].GetInt();
    gameServer.m_iPort =  gameinfo["port"].GetInt();
    gameServer.m_sHost =  gameinfo["host"].GetString();
    m_mServerMap[enServerType::FE_GAMESERVER] = gameServer;

    ServerInfo dbServer;
    Value& dbinfo = m_Obj["dbinfo"];
    dbServer.m_iServerId =  dbinfo["serverid"].GetInt();
    dbServer.m_iPort =  dbinfo["port"].GetInt();
    dbServer.m_sHost =  dbinfo["host"].GetString();
    m_mServerMap[enServerType::FE_DBSERVER] = gameServer;

    ServerInfo proxyServer;
    Value& proxyinfo = m_Obj["proxyinfo"];
    dbServer.m_iServerId =  proxyinfo["serverid"].GetInt();
    dbServer.m_iPort =  proxyinfo["port"].GetInt();
    dbServer.m_sHost =  proxyinfo["host"].GetString();
    m_mServerMap[enServerType::FE_PROXYSERVER] = gameServer;

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
