//
//  main.cpp
//  tcpserver 
//  Created by DGuco on 16/12/8.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <memory>
#include <signal.h>
#include "inc/gatectrl.h"

using namespace std;

CGateCtrl* g_pGateServer;

void sigusr1_handle(int iSigVal)
{
    g_pGateServer->SetRunFlag(reloadcfg);
    signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle(int iSigVal)
{
    g_pGateServer->SetRunFlag(tcpexit);
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
    INIT_ROLLINGFILE_LOG("default","../log/gatesvrd.log",LEVEL_DEBUG);
    CServerConfig* pTmpConfig = new CServerConfig;
    const string filepath = "../config/serverinfo.json";
    if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filepath))
    {
        LOG_ERROR("default","Get TcpserverConfig failed");
        exit(0);
    }

    g_pGateServer = new CGateCtrl;
    iTmpRet = g_pGateServer->Initialize();
    if (0 != iTmpRet)
    {
        delete g_pGateServer;
        LOG_ERROR("default","Tcpserver Initialize failed,iRet = %d",iTmpRet);
        exit(0);
    }

    {
        printf("-------------------------------------------------\n");
        printf("|          gateserver startup success!          |\n");
        printf("-------------------------------------------------\n");
    }
    g_pGateServer->Run();
}
