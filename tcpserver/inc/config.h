//
// Created by dguco on 17-2-16.
//

#ifndef SERVER_CONFIG_H_H
#define SERVER_CONFIG_H_H
#include "../../framework/base/servertool.h"
#include "../../framework/json/json.h"

class CTcpConfig : public CSingleton<CTcpConfig>,public MyJson::Json
{
public:
    CTcpConfig();
    ~CTcpConfig();
    int Parse();

    void Clear();

public:
    int m_iServerId;
    string m_sHost;
    int m_iPort;
};

#endif //SERVER_CONFIG_H_H
