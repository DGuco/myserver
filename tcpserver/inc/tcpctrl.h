//
//  commdef.cpp
//  客户端tcp管理类头文件
//  Created by 杜国超 on 16/12/6.
//  Copyright © 2016年 杜国超. All rights reserved.
//

#ifndef __TCPCTRL_H__
#define __TCPCTRL_H__

#include <sys/epoll.h>
#include "../../common/tools/inc/tcp_conn.h"
#include "../../common/tools/inc/servertool.h"
#include "../../common/message/tcpmessage.pb.h"
#include "../../common/message/message.pb.h"
#include "../../common/tools/inc/protoconfig.h"
#include "../inc/commdef.h"

#define MAX_ERRNO_NUM 10
#define READSTAT      0
#define WRITESTAT     1

typedef struct
{
    int     m_iConnIncoming;
    int     m_iConnTotal;
    int		m_iPkgRecv;
    int		m_iPkgSend;
    int		m_iPkgSizeRecv;
    int		m_iPkgSizeSend;
} TTcpStat;

typedef CTCPConn<RECVBUFLENGTH,POSTBUFLENGTH> MyTcpConn;
