//
//  config.h
//  读取配置文件
//  Created by dguco on 17-2-16.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef SERVER_CONFIG_H_H
#define SERVER_CONFIG_H_H
#include "../base/servertool.h"
#include "json.h"

class CServerConfig : public CSingleton<CServerConfig>,public MyJson::Json
{
public:
    CServerConfig();
    ~CServerConfig();
    int Parse();

    void Clear();

public:
    //TCPserver
    int m_iTcpServerId;
    string m_sTcpHost;
    int m_iTcpPort;

    //gateserver
    int m_iGateServerId;
    string m_sGateHost;
    int m_iGatePort;

    //gameserver
    int m_iGameServerId;
    string m_sGameHost;
    int m_iGamePort;

    //心跳
    int m_iTcpKeepAlive;
    int m_iServerTick;
};

template<class CServerConfig> CServerConfig* CSingleton<CServerConfig>::spSingleton = NULL;
#endif //SERVER_CONFIG_H_H
