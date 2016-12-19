//
//  main.cpp
//  tcpserver 
//  Created by DGuco on 16/12/8.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <iostream>
#include <signal.h>
#include <stdio.h>
#include "inc/tcpctrl.h"
#include "inc/commdef.h"

using namespace std;

CTcpCtrl *g_pTcpServer;

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

    g_pTcpServer = new CTcpCtrl;
    if (g_pTcpServer == NULL)
    {
        LOG_ERROR("default","New TcpCtrl failed.");
        exit(0);
    }

    iTmpRet = g_pTcpServer->Initialize();
    if (0 != iTmpRet)
    {
        LOG_ERROR("default","Tcpserver Initialize failed,iRet = %d",iTmpRet);
        delete g_pTcpServer;
        g_pTcpServer = NULL;
        exit(0);
    }
    LOG_INFO("default", "tcp server is going to run...");
    std::cout << "*********************" << std::endl;
    std::cout << "*********************" << std::endl;
    std::cout << "TcpServer start success" << std::endl;
    std::cout << "*********************" << std::endl;
    std::cout << "*********************" << std::endl;

    g_pTcpServer->Run();

    if (g_pTcpServer != NULL)
    {
        delete g_pTcpServer;
        g_pTcpServer = NULL; 
    }
}
