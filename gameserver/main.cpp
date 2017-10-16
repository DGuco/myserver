//
//  main.cpp
//  gameserver
//  Created by DGuco on 17-3-1.
//  Copyright © 2017年 DGuco. All rights reserved.
//

#include <stdio.h>
#include <signal.h>
#include "inc/gameserver.h"
#include "../framework/json/config.h"
#include "../framework/thread/threadpool.h"

void sigusr1_handle(int iSigVal)
{
    CGameServer::GetSingletonPtr()->SetRunFlag(ERF_RELOAD);
    signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle(int iSigVal)
{
    CGameServer::GetSingletonPtr()->SetRunFlag(ERF_QUIT);
    signal(SIGUSR2, sigusr2_handle);
}

int main(int argc, char* argv[])
{
    CThreadPool pool(4);
    std::vector< std::future<int> > results;

    for(int i = 0; i < 8; ++i) {
        results.emplace_back(
                pool.PushTaskBack([i] {
                    std::cout << "hello " << i << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    std::cout << "world " << i << std::endl;
                    return i*i;
                })
        );
    }

    for(auto && result: results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;

    return 0;

    // 初始化日志
    INIT_ROLLINGFILE_LOG("default", "../log/gameserver.log", LEVEL_DEBUG);

    // 读取配置
    CServerConfig* pTmpConfig = new CServerConfig;
    const string filepath = "../config/serverinfo.json";
    if (-1 == CServerConfig::GetSingleton().LoadFromFile(filepath))
    {
        LOG_ERROR("default","Get TcpserverConfig failed");
        delete pTmpConfig;
        pTmpConfig = NULL;
        exit(0);
    }

    LOG_INFO("default", "---------- printf server config ----------");

    // 准备启动服务器
    CGameServer* pTmpGameServer = new CGameServer;
    if (pTmpGameServer == NULL)
    {
        printf("new CGameServer failed.\n");
        exit(0);
    }

    int iRet = pTmpGameServer->PrepareToRun();
    if (iRet != 0)
    {
        printf("CGameServer prepare to run failed, iRet = %d.\n", iRet);
        exit(0);
    }

    // 信号
    signal(SIGUSR1, sigusr1_handle);
    signal(SIGUSR2, sigusr2_handle);

    {
        printf("-------------------------------------------------\n");
        printf("|          gameserver startup success!          |\n");
        printf("-------------------------------------------------\n");
    }

    // 启动服务器
    pTmpGameServer->Run();

    if (pTmpGameServer)
    {
        delete pTmpGameServer;
        pTmpGameServer = NULL;
    }

    // 关闭日志
    LOG_SHUTDOWN_ALL;
    return 0;
}

