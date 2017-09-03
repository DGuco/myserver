//
//  config.h
//  读取配置文件
//  Created by dguco on 17-2-16.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef SERVER_CONFIG_H_H
#define SERVER_CONFIG_H_H
#include <memory>
#include <map>
#include "../base/servertool.h"
#include "json.h"
#include "../base/commondef.h"

struct ServerInfo {
    int m_iServerId;
    string m_sHost;
    int m_iPort;
};

class CServerConfig : public CSingleton<CServerConfig>,public MyJson::Json
{
public:
    CServerConfig();
    ~CServerConfig();
    int Parse();
    void Clear();

public:
    const std::map<EServerType ,ServerInfo>& GetServerMap() const {return m_mServerMap;}
    const string GetDbInfo() const {return m_sDblInfo;}
    const string GetDbIp() const { return m_sDbIp;}
    const int GetDbSleep() const {return m_iDbSleepTime;}
    const int GetDbLoop() const { return m_iDbLoop;}

    const int GetTcpKeepAlive() const {return m_iTcpKeepAlive;}
    const int GetServetTick() const {return m_iServerTick;}
    const int GetCheckTimeOutGap() const {return m_iChecktimeOutGap;}
    const int GetSocketTimeOut() const {return m_iSokcetTimeout;}
    const int GetProxySize() const { return m_iProxySize;}

private:
    string m_sDbIp;
    string m_sDblInfo;
    int m_iDbSleepTime;
    int m_iDbLoop;

    //心跳
    int m_iTcpKeepAlive;
    int m_iServerTick;
    int m_iChecktimeOutGap;
    int m_iSokcetTimeout;
    int m_iProxySize;
    std::map<EServerType ,ServerInfo> m_mServerMap;
};

template<class CServerConfig> CServerConfig* CSingleton<CServerConfig>::spSingleton = NULL;
#endif //SERVER_CONFIG_H_H
