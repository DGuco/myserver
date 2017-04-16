//
//  main.cpp
//  tcpserver 
//  Created by DGuco on 16/12/8.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <memory>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include "inc/tcpctrl.h"
#include "inc/commdef.h"
#include "../framework/json/config.h"

using namespace std;

unique_ptr<CTcpCtrl> g_pTcpServer;

void sigusr1_handle(int iSigVal)
{
    g_pTcpServer->SetRunFlag(reloadcfg);
    signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle(int iSigVal)
{
    g_pTcpServer->SetRunFlag(tcpexit);
    signal(SIGUSR2, sigusr2_handle);
}

void Initialize()
{
    signal(SIGUSR1,sigusr1_handle);
    signal(SIGUSR2,sigusr2_handle);
}


int main(int argc,char **argv)
{
    int iTmpRet;
    Initialize();
    //初始化日志
    INIT_ROLLINGFILE_LOG("default","../log/tcpsvrd.log",LEVEL_DEBUG);
    unique_ptr<CServerConfig> pTmpConfig(new CServerConfig);
    const string filepath = "../config/serverinfo.json";
    if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filepath))
    {
        LOG_ERROR("default","Get TcpserverConfig failed");
        exit(0);
    }

    unique_ptr<CTcpCtrl> pTmpTcpCtrl(new CTcpCtrl);
    g_pTcpServer = move(pTmpTcpCtrl);
    if (g_pTcpServer == NULL)
    {
        LOG_ERROR("default","New TcpCtrl failed.");
        exit(0);
    }

    iTmpRet = g_pTcpServer->Initialize();
    if (0 != iTmpRet)
    {
        LOG_ERROR("default","Tcpserver Initialize failed,iRet = %d",iTmpRet);
        exit(0);
    }

    LOG_INFO("default", "tcp server is going to run...");

    g_pTcpServer->Run();

}
