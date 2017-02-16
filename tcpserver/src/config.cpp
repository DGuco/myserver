//
// Created by dguco on 17-2-16.
//
#include "../inc/config.h"

template<> CTcpConfig* CSingleton<CTcpConfig>::spSingleton = NULL;

CTcpConfig::CTcpConfig()
{
    m_iPort = 0;
    m_iServerId = 0;
}

CTcpConfig::~CTcpConfig()
{

}

int CTcpConfig::Parse()
{
    m_iServerId = m_Obj["serverid"].GetInt();
    m_iPort = m_Obj["port"].GetInt();
    m_sHost = m_Obj["host"].GetString();
    return 0;
}

void CTcpConfig::Clear()
{

}
