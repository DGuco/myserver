//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
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
#include "../inc/commdef.h"

/**
  * 函数名          : CTcpCtrl::CTcpCtrl
  * 功能描述        : CTCPCtrl构造函数
  * 返回值          ：无 
**/
CTcpCtrl::CTcpCtrl()
{
}

/**
  * 函数名          : CTcpCtrl::~CTcpCtrl
  * 功能描述        : CTCPCtrl析造函数
  * 返回值          ：无 
**/
CTcpCtrl::~CTcpCtrl()
{
}

/**
  * 函数名          : CTCPCtrl::Initialize
  * 功能描述        : 初始化CTCPCtrl对象的各成员变量
  * 返回值         ： int(成功：0 失败：错误码)
**/
int CTcpCtrl::Initialize()
{
    int iTmpI;
    int iTmpRet;
    BYTE* pbTmp;

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
        memset(&mastSocketInfo[i],0,sizeof(TSocketInfo));
        //把socket句柄设为无效句柄
        mastSocketInfo[i].miSocket = INVALID_SOCKET;
    }

    //初始化epoll
    mpEpollevents = NULL;
    //初始化epoll socket
    iTmpRet = InitEpollSocket((short)listenport);
    if (0 != iTmpRet)
    {
        LOG_ERROR("default","InitEpollSocket failed! TCPserver init failed. ReusltCode = %d!",iTmpRet);
        return iTmpRet;   
    }
    LOG_INFO("default","InitEpollSocket successed! TCPserver init failed. ReusltCode = %d!",iTmpRet);

    mastSocketInfo[miSocket].miSocket = miSocket;
    mastSocketInfo[miSocket].miSocketType = LISTEN_SOCKET;
    mastSocketInfo[miSocket].miSocketFlag = RECV_DATA;
    mastSocketInfo[miSocket].miConnectedPort = listenport;
    miMaxfds = miSocket++;

    return 0;
}

int CTcpCtrl::Run()
{
    LOG_NOTICE("default","Tcpserver is runing....");
#ifdef _DEBUG_
    time_t tNow = 0;
    time_t tTick = 0;
    int iTmpCnt = 0;
#endif 

    while(1)
    {
        if (tcpexit == miRunFlag)
        {
            LOG_NOTICE("default","TcpServer exit!");
            return 0;
        }

        if(reloadcfg == miRunFlag)
        {
            LOG_NOTICE("default","Reload tcpsvrd config file ok!");
            miRunFlag = 0;
        }
       
       GetExMessage();              //读取客户端输入
    }
    
    return 0;
}

int CTcpCtrl::SetRunFlag(int iRunFlag)
{
    return 0;
}

/**
  * 函数名          : CTCPCtrl::InitEpollSocket
  * 功能描述        : 初始化Epoll socket
  * 返回值          ：void 
**/
int CTcpCtrl::InitEpollSocket(short shTmpport)
{
    int iTmpRet = 0;
    socklen_t iOptval = 0;
    //兼容不同的平台系统
    int iOpenLen = sizeof(int);
    int iTmpFlags = 0;
    struct linger ling = {0,0};
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(struct sockaddr_in));

    if (EphInit() == -1)
    {
        return 2;
    }

    if ((miSocket = EphSocket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        EphCleanUp();
        return 3;
    }

    /*SO_REUSEADDR
     *R允许启动一个监听服务器并捆绑其众所周知端口，即使以前建立的将此端口用做他们的本地端口的连接仍存在。这通常是重启监听服务器时出现，若不设置此选项，则bind时将出错。
     *允许在同一端口上启动同一服务器的多个实例，只要每个实例捆绑一个不同的本地IP地址即可。对于TCP，我们根本不可能启动捆绑相同IP地址和相同端口号的多个服务器。
     *允许单个进程捆绑同一端口到多个套接口上，只要每个捆绑指定不同的本地IP地址即可。这一般不用于TCP服务器。
     *允许完全重复的捆绑：当一个IP地址和端口绑定到某个套接口上时，还允许此IP地址和端口捆绑到另一个套接口上。一般来说，这个特性仅在支持多播的系统上才有，而且只对UDP套接口而言（TCP不支持多播）。
    */
    setsockopt(miSocket,SOL_SOCKET,SO_REUSEADDR,&iTmpFlags,sizeof(iTmpFlags));
    setsockopt(miSocket,SOL_SOCKET,SO_KEEPALIVE,&iTmpFlags,sizeof(iTmpFlags));
    //close()立刻返回，底层将未发送完的数据发送完成后再释放资源，即优雅退出。
    setsockopt(miSocket,SOL_SOCKET,SO_LINGER,&ling,sizeof(linger));
    //禁止Nagle’s Algorithm延时算法，立刻发送
    setsockopt(miSocket,IPPROTO_TCP,TCP_NODELAY,&iTmpFlags,sizeof(iTmpFlags));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(shTmpport);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(miSocket,(struct sockaddr *)&addr,sizeof(addr)) == -1)
    {
        printf("******** error %s",strerror(errno));
        LOG_ERROR("default","Bind socket Error!");
        EphClose(miSocket);
        EphCleanUp();
        return 4;
    }

    int iTmpOptLen = sizeof(socklen_t);
    int iTmpOptval = TCP_BUFFER_LEN;
    if (setsockopt(miSocket,SOL_SOCKET,SO_RCVBUF,(const void*)&iTmpOptval,iTmpOptLen))
    {
        printf("******** error %s",strerror(errno));
        LOG_ERROR("default","Set Recv buffer size to %d failed",iTmpOptval);
        return -1;
    }

    if (!getsockopt(miSocket,SOL_SOCKET,SO_RCVBUF,(void*)&iTmpOptval,(socklen_t*)&iTmpOptLen))
    {
        LOG_INFO("default","Set Recv buffer size to %d",iTmpOptval);
    }
    //设置接受队列大小
    iTmpRet = listen(miSocket,RECV_QUEUQ_MAX);
    if (-1 == iTmpRet)
    {
        LOG_ERROR("default","Listen %d connection failed",RECV_QUEUQ_MAX);
        return -1;
    }

    //把epoll socket fd放入epoll socket集合中
    if (EphNewConn(miSocket) == -1)
    {
        EphClose(miSocket);
        EphCleanUp();
        return 5;
    }
}

/**
  * 函数名          : CTCPCtrl::EphInit
  * 功能描述        : 初始化Epoll event
  * 返回值          ：void 
**/
int CTcpCtrl::EphInit()
{
    //已经设置
    if (NULL != mpEpollevents)
    {
        return 0;
    }

    memset(&mstEpollEvent,0,sizeof(struct epoll_event));
    mstEpollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP; //epoll监听 可读，错误，和挂起事件
    //初始化event触发事件信息
    mstEpollEvent.data.ptr = NULL;
    mstEpollEvent.data.fd = -1;

    mpEpollevents = (struct epoll_event*)malloc((MAX_SOCKET_NUM) * sizeof(struct epoll_event));
    if (NULL == mpEpollevents)
    {
        LOG_ERROR("default","malloc mpEpollevents Error!");
        return -1;
    }
    //生成epoll描述符
    if ((miKdpfd = epoll_create(MAX_SOCKET_NUM)) < 0)
    {
        LOG_ERROR("default","ERROR:%s",strerror(errno));
        LOG_ERROR("default","epoll create error!");
        return -1;
    }
    return 0;
}

/**
  * 函数名          : CTCPCtrl::EphSocket
  * 功能描述        : 初始化Epoll socket
  * 返回值          ：void 
**/
int CTcpCtrl::EphSocket(int iDomain,int iType,int iProtocol)
{
    int iTmpSfd = socket(iDomain,iType,iProtocol);
    int iTmpFlags = 1;

    if (iTmpSfd == -1)
    {
        LOG_ERROR("default","socket create error!");
        return -1;
    }

    //设置socket非阻塞
    if (ioctl(iTmpSfd,FIONBIO,&iTmpFlags) &&
        ((iTmpFlags = fcntl(iTmpSfd,F_GETFL,0)) < 0 || fcntl(iTmpSfd,F_GETFL, iTmpFlags | O_NONBLOCK) < 0))
    {
        LOG_ERROR("default","set socket nonblock error!");
        close(iTmpSfd);
        return -1;
    }
    return iTmpSfd;
}

/**
  * 函数名          : CTCPCtrl::EphSocket
  * 功能描述        : 关闭Epoll socket
  * 返回值          ：void 
**/
int CTcpCtrl::EphClose(int iSocketFd)
{
    close(iSocketFd);
    return 0;
}

/**
  * 函数名          : CTCPCtrl::EphCleanUp
  * 功能描述        : 清除epoll
  * 返回值          ：void 
**/
int CTcpCtrl::EphCleanUp()
{
    free(mpEpollevents);
    close(miKdpfd);
    mpEpollevents = NULL;
    return 0;
}

/**
  * 函数名          : CTCPCtrl::EphNewConn
  * 功能描述        : 创建连接
  * 返回值          ：void 
**/
int CTcpCtrl::EphNewConn(int iSocketFd)
{
    mstEpollEvent.data.fd = iSocketFd;
    if (epoll_ctl(miKdpfd,EPOLL_CTL_ADD,iSocketFd,&mstEpollEvent) < 0)
    {
        LOG_ERROR("default","create new connection error,socket fd:%d!",iSocketFd);
        return -1;
    }
    return 0;
}

/**
  * 函数名          : CTCPCtrl::GetExMessage
  * 功能描述        : 接收外部数据
  * 返回值          ：void
**/
int CTcpCtrl::GetExMessage()
{
    int  iTmpRet;
    socklen_t iTmpSocketAddrSize;
    int iTmpNewSocket;
    int iTmpFd;
    int iTmpfdNum;
    int iTmpFlags;
    struct epoll_event* pevents;
    iTmpSocketAddrSize = sizeof(mstSockAddr);
    iTmpfdNum = epoll_wait(miKdpfd,mpEpollevents,MAX_SOCKET_NUM,miTimeout);
    iTmpFlags = 1;
    if (-1 == iTmpfdNum)
    {
        LOG_ERROR("default","Epoll wait error,return -1");
        return 0;
    }
    int i,j;
    for (i = 0,pevents = mpEpollevents; i < iTmpfdNum;i++ ,pevents++)
    {
        iTmpFd = pevents->data.fd;
        mpSocketInfo = &mastSocketInfo[iTmpFd];
        //无效文件描述符
        if (iTmpFd <= 0)
        {
            LOG_ERROR("default","Epoll event->data.fd = %d ,error",pevents->data.fd);
            continue;
        }

        //epoll错误（注：同或运算）
        if ((EPOLLERR & pevents->events) != 0)
        {
            LOG_ERROR("default","Epoll event->data.fd = %d ,error",pevents->data.fd);
            continue;
        }

        //epoll没有监听可写事件
        if ((EPOLLIN & pevents->events) == 0)
        {
            LOG_ERROR("default","Epoll dosen't listen input event");
            continue;            
        }
        
        if (mpSocketInfo->miSocketType == LISTEN_SOCKET)
        {
            LOG_NOTICE("default","recv events:%d fd:%d",iTmpfdNum,iTmpFd);
            //accept 一个tcp连接
            iTmpNewSocket = accept(iTmpFd,(struct sockaddr*) &mstSockAddr,(socklen_t*) &iTmpSocketAddrSize);
            //客户端主动关闭了连接
            if (iTmpNewSocket <= 0)
            {
                LOG_NOTICE("default","client canncled connection: port %d fd:%d,errno(%d : %s)",
                            mpSocketInfo->miConnectedPort,iTmpNewSocket,errno,strerror(errno));
                continue;
            }

            mstTcpStat.miConnIncoming ++;
            if (iTmpNewSocket >= MAX_SOCKET_NUM)
            {
                LOG_ERROR("default","socket is too big %d",iTmpNewSocket);
                //关闭socket连接
                closesocket(iTmpNewSocket);
                continue;
            }

            //设置socket为非阻塞
            if (ioctl(iTmpNewSocket, FIONBIO, &iTmpFlags) &&
                ((iTmpFlags = fcntl(iTmpNewSocket, F_GETFL, 0)) < 0 ||
                 fcntl(iTmpNewSocket, F_SETFL, iTmpFlags | O_NONBLOCK) < 0))
            {
                LOG_ERROR("default","operate on socket %d error connect port %d!", iTmpNewSocket, mpSocketInfo->miConnectedPort);
                closesocket(iTmpNewSocket);
                continue;
            }
            //把socket 添加到epoll event 监听集合中，监听可读事件（这里只监听可读事件）
            iTmpRet = EphNewConn(iTmpNewSocket);
            if (iTmpRet != 0)
            {
                LOG_ERROR("default","add to epoll failed [socket %d connect port %d]!", iTmpNewSocket,mpSocketInfo->miConnectedPort);
                closesocket(iTmpNewSocket);
                continue;
            }

            //更改当前的最大socket
            if (iTmpNewSocket > miMaxfds)
            {
                miMaxfds = iTmpNewSocket;
            }
            // 总连接数加1
            mstTcpStat.miConnTotal ++;

            //生成一个socket结构
            j = iTmpNewSocket;
            char* pTmpIp = inet_ntoa(mstSockAddr.sin_addr);
            mastSocketInfo[j].miSrcIP = mstSockAddr.sin_addr.s_addr;
            mastSocketInfo[j].mnSrcPort = mstSockAddr.sin_port;
            strncpy(mastSocketInfo[j].mszClientIP,pTmpIp,sizeof(mpSocketInfo[j].mszClientIP) -1);
            time(&(mastSocketInfo[j].mtCreateTime));
            mastSocketInfo[j].mtStamp = mastSocketInfo[j].mtCreateTime;
            mastSocketInfo[j].miSocketType = CONNECT_SOCKET;
            mastSocketInfo[j].miSocket = iTmpNewSocket;
            mastSocketInfo[j].miSocketFlag = RECV_DATA;
            mastSocketInfo[j].miConnectedPort  = mpSocketInfo->miConnectedPort;
            mastSocketInfo[j].miUin = 0;
            LOG_NOTICE("default","%s connected port %d, socket id = %d.", pTmpIp, mpSocketInfo->miConnectedPort, iTmpNewSocket);
        }
        else      //接收客户端数据
        {
            mpSocketInfo = &mastSocketInfo[iTmpFd];
            RecvClientData(iTmpFd);
        }

    }
}

/**
  * 函数名          : CTCPCtrl::RecvClientData
  * 功能描述        : 接客户端的数据
  * 返回值          ：void
**/
int CTcpCtrl::RecvClientData(int iSocketFd)
{
    int iTmpRet = 0;
    int iTmpRecvBytes = 0;
    int iTempRet;
    int iRecvBytes;
    int iTmpOffset;
    char* pTemp;
    char* pTemp1;
    unsigned short unRecvLen;
    int nRecvAllLen;
    time_t tTempTime;
    int iTmpSocket = mpSocketInfo->miSocket;
    unsigned short unLength = 0;
    iTmpOffset = mpSocketInfo->miRecvBytes; 
    
    //读取tcp数据
    iTmpRecvBytes = TcpRead(iTmpSocket,mpSocketInfo->mszMsgBuf + iTmpOffset,sizeof(mpSocketInfo->mszMsgBuf) - iTmpOffset); 
    //客户端关闭连接
    if (iTmpRecvBytes <= 0)
    {
        LOG_ERROR("default","Client[%s] close the tcp connection,socket id = %d,the client's port = ",
            mpSocketInfo->mszClientIP,mpSocketInfo->miSocket,mpSocketInfo->miConnectedPort);
        ClearSocketInfo(Err_ClientClose);
        return -1;
    }

    //统计接受信息
    mstTcpStat.miPkgSizeRecv += iTmpRecvBytes;
    //增加收到的总字节数
    mpSocketInfo->miRecvBytes = mpSocketInfo->miRecvBytes + iTmpRecvBytes;
    //记录消息起始地址
    pTemp1 = mpSocketInfo->mszMsgBuf;
    nRecvAllLen = mpSocketInfo->miRecvBytes;

    // 记录该socket接收客户端数据的时间
    time(&tTempTime);
    mpSocketInfo->mtStamp = tTempTime;

    while(1)
    {
        if ( nRecvAllLen < MSG_BASE_LEN)
        {
            LOG_ERROR("default","the package len is less than base len ,receive len %d",nRecvAllLen);
            // 最小接受到的客户端的消息字节数不会小于8字节
            // 这里之所以是8字节不是2字节不是因为消息会超过2字节大小
            //主要是因为做了8字节对齐
            break;
        }

        //取出包的总长度
        memcpy(&unRecvLen,(void*)pTemp1,sizeof(unsigned short));
        if (unRecvLen < MSG_BASE_LEN || unRecvLen > MSG_OPT_LEN)
        {
            LOG_ERROR("default","the package len is illegal",nRecvAllLen);
            ClearSocketInfo(Err_PacketError);
            return -1;
        }
        nRecvAllLen -= unRecvLen;
        //数据指针向后移动指向未读取位置
        pTemp1 += unRecvLen;
        //总长度小于包长的长度,结束
        if(nRecvAllLen < 0)
        {
            nRecvAllLen += unRecvLen;
            pTemp1 -= unRecvLen;
            LOG_DEBUG("default", "Receive client part data left len = %d",nRecvAllLen,unRecvLen);
            break;
        }

        unLength = 0;
        //包长数据已读出,准备都去真正的数据存入mszMsgBuf
        pTemp = mszMsgBuf;

        //预留总长度
        pTemp += sizeof(short);
        unLength += sizeof(short);

        //预留8字节对齐长度
        pTemp += sizeof(short);
        unLength += sizeof(short);

        CMessage tmpMsg;
        tmpMsg.Clear();

        iTmpRet = ClientCommEngine::ConvertStreamToMsg(pTemp1 - unRecvLen,unRecvLen,&tmpMsg);
        if (iTmpRet != 0)
        {
            LOG_ERROR("default","CTCPCtrl::RecvClientData error,ConvertStreamTomsg return %d",iTmpRet);
            ClearSocketInfo(Err_PacketError);
            return -1;
        }

        if (tmpMsg.msghead().messageid() != CMsgPingRequest::MsgID)
        {
            //转发消息
            CTcpHead pbTmpTcpHead;
            pbTmpTcpHead.Clear();
            pbTmpTcpHead.set_srcfe(FE_TCPSERVER);      //设置源服务器
            pbTmpTcpHead.set_srcid(tcpserverid);
            pbTmpTcpHead.set_dstfe(tmpMsg.msghead().dstfe());
            pbTmpTcpHead.set_srcid(tmpMsg.msghead().dstid());
            pbTmpTcpHead.set_timestamp(tTempTime);
            CSocketInfo* pbSocketInfo = pbTmpTcpHead.add_socketinfos();
            if (pbSocketInfo)
            {
                LOG_ERROR("default","CTCPCtrl::RecvClientData error,add socketingo error,pbSocketInfo is NULL");
                ClearSocketInfo(Err_PacketError);
            }

            pbSocketInfo->set_createtime(mpSocketInfo->mtCreateTime);
            pbSocketInfo->set_socketid(iSocketFd);
            pbSocketInfo->set_createtime(mpSocketInfo->mtCreateTime);
            pbSocketInfo->set_srcip(mpSocketInfo->miSrcIP);
            pbSocketInfo->set_srcport(mpSocketInfo->mnSrcPort);
            //state < 0 说明关闭socket
            pbSocketInfo->set_state(0);

            //序列化CTCPhead
            *(short*)pTemp = pbTmpTcpHead.ByteSize();
            pTemp += sizeof(short);
            unLength += sizeof(short);

            //序列化CTCPhead
            if (pbTmpTcpHead.SerializeToArray(pTemp, sizeof(mszMsgBuf) - unLength - 1) != true)
            {
                LOG_ERROR("default", "CTCPCtrl::RecvClientData error,pbTmpTcpHead SerializeToArray error");
                ClearSocketInfo(Err_PacketError);
                return -1;
            }

            pTemp += pbTmpTcpHead.GetCachedSize();
            unLength += pbTmpTcpHead.GetCachedSize();

            //拷贝消息到发送缓冲区
            memcpy(pTemp,pTemp1 - unRecvLen,unRecvLen);
            pTemp += unRecvLen;
            unLength += unRecvLen;

            //8字节对齐
            unsigned short iTmpAddlen = (unLength % 8);
            if (iTmpAddlen > 0)
            {
                iTmpAddlen = 8 - iTmpAddlen;
                memset(pTemp,0,iTmpAddlen);
            }

            pTemp += iTmpAddlen;
            unLength += iTmpAddlen;
            //序列话消息总长度
            pTemp = mszMsgBuf;
            *(short*) pTemp = unLength;
            pTemp += sizeof(short);

            //序列话8字节对齐长度
            *(short*) pTemp = iTmpAddlen;

        }
    }
}

/**
  * 函数名          : CTCPCtrl::TcpRead
  * 功能描述        : 读取tcp数据
  * 返回值          ：void
**/
int CTcpCtrl::TcpRead(int iSocket, char *pBuf, int iLen)
{
    int iTmpRecvBytes = 0;
    while(1)
    {
        iTmpRecvBytes = read(iSocket,pBuf,iLen);
        //读取成功
        if (iTmpRecvBytes > 0)
        {
            return iTmpRecvBytes;
        }
        else
        {
            if (iTmpRecvBytes < 0 && errno == EINTR)
            {
                continue;
            }
            return iTmpRecvBytes;
        }
    }
}

/**
  * 函数名          : CTCPCtrl::TcpRead
  * 功能描述        : 读取tcp数据
  * 返回值          ：void
**/
void CTcpCtrl::ClearSocketInfo(short enError)
{
    int iTmpRet;
    if ( TCP_SUCCESS == enError)
    {}

    //关闭socket
    if (mpSocketInfo->miSocket > 0)
    {
        mstEpollEvent.data.fd = mpSocketInfo->miSocket;
        if (epoll_ctl(miKdpfd,EPOLL_CTL_DEL,mpSocketInfo->miSocket,&mstEpollEvent) < 0)
        {
            LOG_ERROR("default","epoll remove socket error,socket fd = %d",mpSocketInfo->miSocket);
        }
        //关闭socket
        closesocket(mpSocketInfo->miSocket);
        //更改当前对大分配socket
        if(mpSocketInfo->miSocket >= miMaxfds)
        {
            int iTmpUnUseSocket;
            for (iTmpUnUseSocket = mpSocketInfo->miSocket -1 ;iTmpUnUseSocket >= miSocket;iTmpUnUseSocket--)
            {
                if(mastSocketInfo[iTmpUnUseSocket].miSocketFlag != 0)
                {
                    break;
                }
                miMaxfds = iTmpUnUseSocket;
            }
        }
        //总连接数减一
        mstTcpStat.miConnTotal --;
    }
    mpSocketInfo->miSocket = INVALID_SOCKET;
    mpSocketInfo->miSrcIP = 0;
    mpSocketInfo->mnSrcPort = 0;
    mpSocketInfo->miDstIP= 0;
    mpSocketInfo->mnDstPort = short(-1);
    mpSocketInfo->miRecvBytes = 0;
    mpSocketInfo->miSocketType = 0;
    mpSocketInfo->miSocketFlag = 0;
    mpSocketInfo->mtCreateTime = 0;
    mpSocketInfo->mtStamp = 0;
    mpSocketInfo->miSendFlag = 0;
    mpSocketInfo->miConnectedPort = 0;
}
