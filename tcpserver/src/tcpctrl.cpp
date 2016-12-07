//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by 杜国超 on 16/12/6.
//  Copyright © 2016年 杜国超. All rights reserved.
//

#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>
#include <netinet/tcp.h>

#include "../inc/tcpctrl.h"
#include "../../common/tools/inc/client_comm_engine.h"

CTcpCtrl::CTcpCtrl()
{
}

CTcpCtrl::~CTcpCtrl()
{
}

/**
  *函数名          : CTCPCtrl::Initialize
  *功能描述        : 初始化socket和CTCPCtrl对象的各成员变量
  *返回值         ： int(成功：0 失败：错误码)
**/
int CTcpCtrl::Initialize()
{
    int iTmpI;
    int iTmpRet;
    BYTE* pbTmp;
    int iTmpIndex;

    mLastKeepaliveTime = 0;
    miRunFlag = 0;
    miWriteStatCount = 0;
    mSCTcpHead.Clear();
    miSendIndex = 0;
    mbHasRecv = 0;

    memset(&mstTcpStat,0,sizeof(mstTcpStat));

    //初始化客户端socket数组信息
    for (int i = 0; i < MAX_SOCKET_NUM; ++i)
    {
        memset(&mastSocketInfo[i],0,sizeof(TSocketInfo));
        //把socket句柄设为无效句柄
        mastSocketInfo[i].miSocket = INVALID_SOCKET;
    }

    //初始化epoll
    mpEpollevents = NULL;

    return 0;
}

int CTcpCtrl::Run()
{
    return 0;
}

int SetRunFlag(int iRunFlag)
{
    return 0;
}
