//
// Created by dguco on 17-2-16.
//
#include "config.h"

CServerConfig::CServerConfig()
{
    Clear();
}

CServerConfig::~CServerConfig()
{

}

int CServerConfig::Parse()
{
    m_iTcpKeepAlive = m_Obj["keepalive"].GetInt();
    m_iServerTick = m_Obj["servertick"].GetInt();
    m_iChecktimeOutGap = m_Obj["checktimeOutGap"].GetInt();
    m_iSokcetTimeout = m_Obj["socketTimeout"].GetInt();
    m_iProxySize = m_Obj["proxySize"].GetInt();

    ServerInfo tcpServer;
    rapidjson::Value& tcpinfo = m_Obj["tcpinfo"];
    tcpServer.m_iPort =  tcpinfo["port"].GetInt();
    tcpServer.m_sHost =  tcpinfo["host"].GetString();
    tcpServer.m_iServerId = tcpinfo["serverid"].GetInt();
    m_mServerMap[EServerType::SERVER_TCP] = tcpServer;

    ServerInfo gameServer;
    rapidjson::Value& gameinfo = m_Obj["gameinfo"];
    gameServer.m_iServerId =  gameinfo["serverid"].GetInt();
    gameServer.m_iPort =  gameinfo["port"].GetInt();
    gameServer.m_sHost =  gameinfo["host"].GetString();
    m_mServerMap[EServerType::SERVER_GAME] = gameServer;

    ServerInfo gateServer;
    rapidjson::Value& gateinfo = m_Obj["gateinfo"];
    gateServer.m_iServerId =  gateinfo["serverid"].GetInt();
    gateServer.m_iPort =  gateinfo["port"].GetInt();
    gateServer.m_sHost =  gateinfo["host"].GetString();
    m_mServerMap[EServerType::SERVER_GATE] = gameServer;

    ServerInfo dbServer;
    rapidjson::Value& dbinfo = m_Obj["dbinfo"];
    dbServer.m_iServerId =  dbinfo["serverid"].GetInt();
    dbServer.m_iPort =  dbinfo["port"].GetInt();
    dbServer.m_sHost =  dbinfo["host"].GetString();
    m_mServerMap[EServerType::SERVER_DB] = gameServer;

    m_sDblInfo = dbinfo["mysqlinfo"].GetString();
    m_iDbSleepTime = dbinfo["sleeptime"].GetInt();
    m_iDbLoop = dbinfo["loop"].GetInt();

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
