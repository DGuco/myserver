//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <string.h>
#include <netinet/tcp.h>
#include "../inc/gatectrl.h"
#include "../../framework/const/commondef.h"
#include "../../framework/net/client_comm_engine.h"
#include "../../framework/mem/codequeue.h"
#include "../../framework/mem/sharemem.h"
#include "../../gameserver/inc/clienthandle.h"

CGateCtrl::CGateCtrl()
{
}

CGateCtrl::~CGateCtrl()
{
}

int CGateCtrl::Initialize()
{
    int iTmpRet;

    m_iRunFlag = 0;
    m_iSendIndex = 0;
    m_bHasRecv = 0;

    memset(&m_stTcpStat,0,sizeof(m_stTcpStat));

    //初始化客户端socket数组信息
    for (int i = 0; i < MAX_SOCKET_NUM; ++i)
    {
        memset(&m_astSocketInfo[i],0,sizeof(TSocketInfo));
        memset(&m_astSocketInfo[i],0,sizeof(TSocketInfo));
        //把socket句柄设为无效句柄
        m_astSocketInfo[i].m_iSocket = INVALID_SOCKET;
    }


    ServerInfo *gateInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GATESERVER);
    //初始化epoll
    m_pEpollevents = NULL;
    //初始化epoll socket
    iTmpRet = InitEpollSocket((short)gateInfo->m_iPort);
    if (0 != iTmpRet)
    {
        LOG_ERROR("default","InitEpollSocket failed! TCPserver init failed. ReusltCode = %d!",iTmpRet);
        return iTmpRet;   
    }
    LOG_INFO("default","InitEpollSocket successed! TCPserver init successed. ReusltCode = %d!",iTmpRet);

    m_astSocketInfo[m_iSocket].m_iSocket = m_iSocket;
    m_astSocketInfo[m_iSocket].m_iSocketType = LISTEN_SOCKET;
    m_astSocketInfo[m_iSocket].m_iSocketFlag = RECV_DATA;
    m_astSocketInfo[m_iSocket].m_iConnectedPort = CServerConfig::GetSingletonPtr()
            ->GetServerInfo(enServerType::FE_GATESERVER)->m_iPort;
    m_iMaxfds = m_iSocket + 1;

    CreatePipe();

    LOG_INFO("default", "GateServer is going to run at %s : %d",gateInfo->m_sHost.c_str(),gateInfo->m_iPort);
    return 0;
}

int CGateCtrl::Run()
{
    LOG_NOTICE("default","Tcpserver is runing....");

    while(1)
    {
        if (tcpexit == m_iRunFlag)
        {
            LOG_NOTICE("default","TcpServer exit!");
            return 0;
        }

        if(reloadcfg == m_iRunFlag)
        {
            LOG_NOTICE("default","Reload tcpsvrd config file ok!");
            m_iRunFlag = 0;
        }
       
        GetExMessage();              //读取客户端输入
        CheckWaitSendData();         //发送缓存数据
        CheckTimeOut();              //检测超时
    }
    return 0;
}

int CGateCtrl::SetRunFlag(int iRunFlag)
{
    m_iRunFlag = iRunFlag;
    return 0;
}

/**
  * 函数名          : CGateCtrl::CreatePipe
  * 功能描述        : 创建和游戏服通信的共享内存管道
  * 返回值         ： int(成功：0 失败：错误码)
**/
int CGateCtrl::CreatePipe()
{
    int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);

    ////////////////////////////////mS2CPipe/////////////////////////////////////////
    system("touch ./scpipefile");
    char* pcTmpSCPipeID = getenv("SC_PIPE_ID");
    int iTmpSCPipeID = 0;
    if (pcTmpSCPipeID)
    {
        iTmpSCPipeID = atoi(pcTmpSCPipeID);
    }
    key_t iTmpKeyS2C = MakeKey("./scpipefile", iTmpSCPipeID);
    BYTE* pbyTmpS2CPipe = CreateShareMem(iTmpKeyS2C, iTempSize);
    MY_ASSERT(pbyTmpS2CPipe != NULL, exit(0));
    CSharedMem::pbCurrentShm = pbyTmpS2CPipe;
    CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyS2C, iTempSize,EIMode::SHM_INIT);
    mS2CPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_S2C);

    ////////////////////////////////mC2SPipe/////////////////////////////////////////
    system("touch ./cspipefile");
    char* pcTmpCSPipeID = getenv("CS_PIPE_ID");
    int iTmpCSPipeID = 0;
    if (pcTmpCSPipeID)
    {
        iTmpCSPipeID = atoi(pcTmpCSPipeID);
    }
    key_t iTmpKeyC2S = MakeKey("./cspipefile", iTmpCSPipeID);
    BYTE* pbyTmpC2SPipe = CreateShareMem(iTmpKeyC2S, iTempSize);
    MY_ASSERT(pbyTmpC2SPipe != NULL, exit(0));
    CSharedMem::pbCurrentShm = pbyTmpC2SPipe;
    CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyC2S, iTempSize,EIMode::SHM_INIT);
    mC2SPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_C2S);
    return 0;
}
/**
  * 函数名          : CGateCtrl::InitEpollSocket
  * 功能描述        : 初始化Epoll socket
  * 返回值          ：int
**/
int CGateCtrl::InitEpollSocket(short shTmpport)
{
    int iTmpRet = 0;
    socklen_t iTmpOptval = 0;
    int iTmpOptLen = sizeof(int);
    int iTmpFlags  =0;
    struct linger ling = {0,0};
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(struct sockaddr_in));

    //建立socket
    if ((m_iSocket = EphSocket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        return 3;
    }

    if (EphInit() == -1)
    {
        EphCleanUp();
        return 2;
    }

    /* SO_REUSEADDR
     * 通知内核，如果端口忙，但TCP状态位于 TIME_WAIT ，可以重用端口。如果端口忙，而TCP状态位
     * 于其他状态，重用端口时依旧得到一个错误信息，指明"地址已经使用中"。如果你的服务程序停止后想
     * 立即重启，而新套接字依旧使用同一端口，此时SO_REUSEADDR 选项非常有用。
     * 当tcpserver作为客户端连接其他服务tcp服务时，如果自己断线重连对方时进入TIME_WAIT状态
     * 此时如果重新绑定端口时会失败，这种状态会持续1-4分钟，设置此选项重用该地址，防止这种情况
     * 发生
    */
    setsockopt(m_iSocket,SOL_SOCKET,SO_REUSEADDR,&iTmpFlags,sizeof(iTmpFlags));
    setsockopt(m_iSocket,SOL_SOCKET,SO_KEEPALIVE,&iTmpFlags,sizeof(iTmpFlags));
    //close()立刻返回，底层将未发送完的数据发送完成后再释放资源，即优雅退出。
    setsockopt(m_iSocket,SOL_SOCKET,SO_LINGER,&ling,sizeof(linger));
    //禁止Nagle’s Algorithm延时算法，立刻发送,防止tcp粘包
    setsockopt(m_iSocket,IPPROTO_TCP,TCP_NODELAY,&iTmpFlags,sizeof(iTmpFlags));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(shTmpport);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_iSocket,(struct sockaddr *)&addr,sizeof(addr)) == -1)
    {
        LOG_ERROR("default","Bind socket Error! %s",strerror(errno));
        EphClose(m_iSocket);
        EphCleanUp();
        return 4;
    }

    iTmpOptLen = sizeof(socklen_t);
    iTmpOptval = TCP_BUFFER_LEN;
    if (setsockopt(m_iSocket,SOL_SOCKET,SO_RCVBUF,(const void*)&iTmpOptval,iTmpOptLen))
    {
        LOG_ERROR("default","Set Recv buffer size to %d failed",iTmpOptval);
        return -1;
    }

    if (!getsockopt(m_iSocket,SOL_SOCKET,SO_RCVBUF,(void*)&iTmpOptval,(socklen_t*)&iTmpOptLen))
    {
        LOG_INFO("default","Set Recv buffer size to %d",iTmpOptval);
    }
    //设置接受队列大小
    iTmpRet = listen(m_iSocket,RECV_QUEUQ_MAX);
    if (-1 == iTmpRet)
    {
        LOG_ERROR("default","Listen %d connection failed",RECV_QUEUQ_MAX);
        return -1;
    }

    //把epoll socket fd放入epoll socket集合中
    if (EphNewConn(m_iSocket) == -1)
    {
        EphClose(m_iSocket);
        EphCleanUp();
        return 5;
    }
    return 0;
}

/**
  * 函数名          : CGateCtrl::EphInit
  * 功能描述        : 初始化Epoll event
  * 返回值          ：int
**/
int CGateCtrl::EphInit()
{
    //已经设置
    if (NULL != m_pEpollevents)
    {
        return 0;
    }

    memset(&m_stEpollEvent,0,sizeof(struct epoll_event));
    m_stEpollEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP; //epoll监听 可读，错误，和挂起事件
    //初始化event触发事件信息
    m_stEpollEvent.data.ptr = NULL;
    m_stEpollEvent.data.fd = -1;
    
    m_pEpollevents = (struct epoll_event*)malloc((MAX_SOCKET_NUM) * sizeof(struct epoll_event));
    if (NULL == m_pEpollevents)
    {
        LOG_ERROR("default","malloc mpEpollevents Error!");
        return -1;
    }
    //生成epoll描述符
    if ((m_iKdpfd = epoll_create(MAX_SOCKET_NUM)) < 0)
    {
        LOG_ERROR("default","epoll create error:%s",strerror(errno));
        close(m_iKdpfd);
        return -1;
    }
    return 0;
}

/**
  * 函数名          : CGateCtrl::EphSocket
  * 功能描述        : 初始化Epoll socket
  * 返回值          ：int
**/
int CGateCtrl::EphSocket(int iDomain,int iType,int iProtocol)
{
    int iTmpSfd = socket(iDomain,iType,iProtocol);
    int iTmpFlags = 1;

    if (iTmpSfd == -1)
    {
        LOG_ERROR("default","socket create error! %s",strerror(errno));
        return -1;
    }

    //设置socket非阻塞
    if ((ioctl(iTmpSfd,FIONBIO,&iTmpFlags) < 0) ||
        ((iTmpFlags = fcntl(iTmpSfd,F_GETFL,0)) < 0 || fcntl(iTmpSfd,F_SETFL, iTmpFlags | O_NONBLOCK) < 0))
    {
        LOG_ERROR("default","set socket nonblock error! %s",strerror(errno));
        close(iTmpSfd);
        return -1;
    }
    return iTmpSfd;
}

/**
  * 函数名          : CGateCtrl::EphSocket
  * 功能描述        : 关闭Epoll socket
  * 返回值          ：int
**/
int CGateCtrl::EphClose(int iSocketFd)
{
    close(iSocketFd);
    return 0;
}

/**
  * 函数名          : CTCPCtrl::EphCleanUp
  * 功能描述        : 清除epoll
  * 返回值          ：int
**/
int CGateCtrl::EphCleanUp()
{
    free(m_pEpollevents);
    close(m_iKdpfd);
    m_pEpollevents = NULL;
    return 0;
}

/**
  * 函数名          : CGateCtrl::EphNewConn
  * 功能描述        : 创建连接
  * 返回值          ：int
**/
int CGateCtrl::EphNewConn(int iSocketFd)
{
    m_stEpollEvent.data.fd  = iSocketFd;
    if (epoll_ctl(m_iKdpfd,EPOLL_CTL_ADD,iSocketFd,&m_stEpollEvent) < 0)
    {
        LOG_ERROR("default","create new connection error,socket fd:%d! error:%s",iSocketFd,strerror(errno));
        return -1;
    }
    return 0;
}

/**
  * 函数名          : CGateCtrl::GetExMessage
  * 功能描述        : 接收外部数据
  * 返回值          ：int
**/
int CGateCtrl::GetExMessage()
{
    int  iTmpRet;
    socklen_t iTmpSocketAddrSize;
    int iTmpNewSocket;
    int iTmpFd;
    int iTmpfdNum;
    int iTmpFlags;
    struct epoll_event* pevents;
    iTmpSocketAddrSize = sizeof(m_stSockAddr);
    iTmpfdNum = epoll_wait(m_iKdpfd,m_pEpollevents,MAX_SOCKET_NUM,m_iTimeout);
    iTmpFlags = 1;
    if (-1 == iTmpfdNum)
    {
        LOG_ERROR("default","Epoll wait error,return -1,error %s",strerror(errno));
        return 0;
    }
    int i,j;
    for (i = 0,pevents = m_pEpollevents; i < iTmpfdNum;i++ ,pevents++)
    {
        iTmpFd = pevents->data.fd;
        //无效文件描述符
        if (iTmpFd <= 0)
        {
            LOG_ERROR("default","Epoll event->data.fd = %d ,error",pevents->data.fd);
            continue;
        }

        //epoll错误（注：同或运算：相同为1 不同为0）
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

        m_pSocketInfo = &m_astSocketInfo[iTmpFd];
        //有新的连接
        if (m_pSocketInfo->m_iSocketType == LISTEN_SOCKET)
        {
            LOG_DEBUG("default","recv events:%d fd:%d",iTmpfdNum,iTmpFd);
            //accept 一个tcp连接
            iTmpNewSocket = accept(iTmpFd,(struct sockaddr*) &m_stSockAddr,(socklen_t*)&iTmpSocketAddrSize);
            //客户端主动关闭了连接
            if (iTmpNewSocket <= 0)
            {
                LOG_NOTICE("default","client canncled connection: port %d fd:%d,errno(%d : %s)",
                            m_pSocketInfo->m_iConnectedPort,iTmpNewSocket,errno,strerror(errno));
                continue;
            }

            m_stTcpStat.m_iConnIncoming++;
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
                LOG_ERROR("default","operate on socket %d error connect port %d!", iTmpNewSocket, m_pSocketInfo->m_iConnectedPort);
                closesocket(iTmpNewSocket);
                continue;
            }
            //把socket 添加到epoll event 监听集合中，监听可读事件（这里只监听可读事件）
            iTmpRet = EphNewConn(iTmpNewSocket);
            if (iTmpRet != 0)
            {
                LOG_ERROR("default","add to epoll failed [socket %d connect port %d]!", iTmpNewSocket,m_pSocketInfo->m_iConnectedPort);
                closesocket(iTmpNewSocket);
                continue;
            }

            //更改当前的最大socket
            if (iTmpNewSocket > m_iMaxfds)
            {
                m_iMaxfds = iTmpNewSocket;
            }
            // 总连接数加1
            m_stTcpStat.m_iConnTotal ++;

            //生成一个socket结构
            j = iTmpNewSocket;
            char* pTmpIp = inet_ntoa(m_stSockAddr.sin_addr);
            m_astSocketInfo[j].m_iSrcIP = m_stSockAddr.sin_addr.s_addr;
            m_astSocketInfo[j].m_nSrcPort = m_stSockAddr.sin_port;
            strncpy(m_astSocketInfo[j].m_szClientIP,pTmpIp,sizeof(m_pSocketInfo[j].m_szClientIP) -1);
            time(&(m_astSocketInfo[j].m_tCreateTime));
            m_astSocketInfo[j].m_tStamp = m_astSocketInfo[j].m_tCreateTime;
            m_astSocketInfo[j].m_iSocketType = CONNECT_SOCKET;
            m_astSocketInfo[j].m_iSocket = iTmpNewSocket;
            m_astSocketInfo[j].m_iSocketFlag = RECV_DATA;
            m_astSocketInfo[j].m_iConnectedPort  = m_pSocketInfo->m_iConnectedPort;
            m_astSocketInfo[j].m_iUin = 0;
            LOG_NOTICE("default","%s connected port %d, socket id = %d.", pTmpIp, m_pSocketInfo->m_iConnectedPort, iTmpNewSocket);
        }
        else      //接收客户端数据
        {
            RecvClientData(iTmpFd);
        }
    }
    return 0;
}

/**
  * 函数名          : CGateCtrl::RecvClientData
  * 功能描述        : 接客户端的数据组织客户端数据为服务器其间数据传格式存入消息包缓冲区
  * 返回值          ：int
**/
int CGateCtrl::RecvClientData(int iSocketFd)
{
    int iTmpRet = 0;
    int iTmpRecvBytes = 0;
    int iTmpOffset;
    int nRecvAllLen;
    time_t tTempTime;
    char* pTemp;
    char* pTemp1;

    m_pSocketInfo = &m_astSocketInfo[iSocketFd];
    int iTmpSocket = m_pSocketInfo->m_iSocket;
    iTmpOffset = m_pSocketInfo->m_iRecvBytes; 
    //读取tcp数据
    iTmpRecvBytes = TcpRead(iTmpSocket,m_pSocketInfo->m_szMsgBuf + iTmpOffset,sizeof(m_pSocketInfo->m_szMsgBuf) - iTmpOffset); 
    //客户端关闭连接
    if (iTmpRecvBytes <= 0)
    {
        LOG_ERROR("default","Client[%s] close the tcp connection,socket id = %d,the client's port = %d",
            m_pSocketInfo->m_szClientIP,m_pSocketInfo->m_iSocket,m_pSocketInfo->m_iConnectedPort);
        ClearSocketInfo(Err_ClientClose);
        return -1;
    }

    //统计接受信息
    m_stTcpStat.m_iPkgSizeRecv += iTmpRecvBytes;
    //增加收到的总字节数
    m_pSocketInfo->m_iRecvBytes = m_pSocketInfo->m_iRecvBytes + iTmpRecvBytes;

    pTemp1 = m_pSocketInfo->m_szMsgBuf;
    nRecvAllLen = m_pSocketInfo->m_iRecvBytes;

    // 记录该socket接收客户端数据的时间
    time(&tTempTime);
    m_pSocketInfo->m_tStamp = tTempTime;

    while(1)
    {
        if (nRecvAllLen <= 0)
        {
            break;
        }
        //客户端上行数据长度(除去头部)
        MSG_LEN_TYPE unTmpDataLen = 0;
        MesHead tmpHead;
        //解析数据包头部信息，解析完成后指针指向数据包的末尾
        iTmpRet = ClientCommEngine::ParseClientStream((const void **)&pTemp1,
                                                    nRecvAllLen,
                                                    &tmpHead,
                                                    unTmpDataLen);
        //继续接收
        if (iTmpRet == 1)
        {
            break;
        }
        if (iTmpRet < 0)
        {
            ClearSocketInfo(Err_PacketError);
            return iTmpRet;
        }
        //组织转发消息
        if (0 == iTmpRet /*&& tmpHead.cmd() != CMsgPingRequest::MsgID */&& unTmpDataLen >= 0)
        {
            CSocketInfo *tmpSocketInfo = tmpHead.mutable_socketinfos()->Add();
            tmpSocketInfo->Clear();
            tmpSocketInfo->set_createtime(m_pSocketInfo->m_tCreateTime);
            tmpSocketInfo->set_socketid(m_pSocketInfo->m_iSocket);
            tmpSocketInfo->set_state(0);

            pTemp = m_szCSMsgBuf;
            MSG_LEN_TYPE tmpSendLen = sizeof(m_szCSMsgBuf);
//            char* pDataBuff = pTemp1;
            iTmpRet = ClientCommEngine::ConverToGameStream(pTemp,
                                                            tmpSendLen,
                                                            pTemp1,
                                                            unTmpDataLen,
                                                            &tmpHead);
            if (iTmpRet != 0)
            {
                ClearSocketInfo(Err_SendToMainSvrd);
            }

            iTmpRet = mC2SPipe->AppendOneCode((const BYTE *)pTemp,tmpSendLen);
            if (iTmpRet < 0)
            {
                LOG_ERROR("default", "CTCPCtrl::RecvClientData error,send data to gate error,error code = %d",iTmpRet);
                ClearSocketInfo(Err_PacketError);
                return iTmpRet;
            }
            #ifdef _DEBUG_
                LOG_DEBUG("defalut","tcp ==>gate [%d bytes]",tmpSendLen);
            #endif
        }
        else
        {
            //心跳信息不做处理
        }
        m_stTcpStat.m_iPkgSizeRecv++;
    }
    //数据发送完
    if (nRecvAllLen == 0)
    {
        m_pSocketInfo->m_iRecvBytes = 0;
    }
    else 
    {
        if ((MSG_MAX_LEN < nRecvAllLen) || (nRecvAllLen < 0))
        {
            LOG_ERROR("default", "the package length is illeagl,the package len = %d",nRecvAllLen);
            ClearSocketInfo(Err_PacketError);
            return -1;
        }
    }
    return 0;
}

/**
  * 函数名          : CGateCtrl::CheckTimeOut
  * 功能描述        : 检测超时
  * 返回值          ：int
**/
int CGateCtrl::CheckTimeOut()
{
    int    i;
    time_t tempTimeGap;
    time(&m_iNowTime); // 计算当前时间

    // 和上次检测时间相比，如果达到了检测间隔则进行检测
    if (m_iNowTime - m_iLastTime < CServerConfig::GetSingleton().GetCheckTimeOutGap())
    {
        return 0;
    }
    // 从第1个真正收发数据的socket结构开始到当前最大分配的socket
    // 判断每个socket通讯超时情况
    for (i = m_iSocket + 1; i <= m_iMaxfds; i++)
    {
        m_pSocketInfo = &m_astSocketInfo[i];
        if (0 == m_pSocketInfo->m_iSocketFlag)
        {
            continue;
        }
        // 如果Mainsvrd已经向该socket发送过数据，则判断最后一个接收包的时间
        if (MAIN_HAVE_SEND_DATA == m_pSocketInfo->m_iSendFlag)
        {
            tempTimeGap = m_iNowTime - m_pSocketInfo->m_tStamp;
            // 把当前时间和最近一次socket收到包的时间相比，如果超过了指定的时间间隔则关闭socket
            if (tempTimeGap >= CServerConfig::GetSingleton().GetSocketTimeOut())
            {
                // 该socket通讯超时
                LOG_ERROR("default","Client[%s] socket id = %d port %d not recv packet %d seconds, Close.",
                          m_pSocketInfo->m_szClientIP, m_pSocketInfo->m_iSocket,
                          m_pSocketInfo->m_iConnectedPort, tempTimeGap);

                ClearSocketInfo(Err_ClientTimeout);
            }
        }
        else
        {
            // 该客户端已经连接上来了，但是Mainsvrd还没有向它发送一个包，这时的超时更短，主要是防止恶意攻击
            tempTimeGap = m_iNowTime - m_pSocketInfo->m_tCreateTime;
            if (CServerConfig::GetSingleton().GetSocketTimeOut() < tempTimeGap)
            {
                // 该socket通讯超时
                LOG_ERROR("default","Client[%s] connect port %d Timeout %d seconds, close!",
                          m_pSocketInfo->m_szClientIP, m_pSocketInfo->m_iConnectedPort, tempTimeGap);

                ClearSocketInfo(Err_ClientTimeout);
            }
        }
    }

    // 更新检测时间
    m_iLastTime = m_iNowTime;

    return 0;
}

/**
  * 函数名          : CGateCtrl::TcpRead
  * 功能描述        : 读取tcp数据
  * 返回值          ：int
**/
int CGateCtrl::TcpRead(int iSocket, char *pBuf, int iLen)
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
  * 函数名          : CGateCtrl::TcpWrite
  * 功能描述        : 发送tcp数据
  * 返回值          ：int
**/
int CGateCtrl::TcpWrite(int iSocket, char *pBuf, int iPackLen)
{
    int iSendBytes;
//    int iLeftLen = iPackLen;

    while(1)
    {
        iSendBytes = write(iSocket, pBuf, iPackLen);
        if (iSendBytes == iPackLen)
        {
            return iSendBytes;
        }
        else
        {
            if (0 >= iSendBytes && EINTR == errno)
            {
                continue;
            }
            return iSendBytes;
        }
    }
}

/**
  * 函数名          : CGateCtrl::ClearSocketInfo
  * 功能描述        : 清楚socket
  * 返回值         ：void
**/
void CGateCtrl::ClearSocketInfo(short enError)
{
    if ( TCP_SUCCESS != enError)
    {
        DisConnect(enError);
    }

    //关闭socket
    if (m_pSocketInfo->m_iSocket > 0)
    {
        m_stEpollEvent.data.fd = m_pSocketInfo->m_iSocket;
        if (epoll_ctl(m_iKdpfd,EPOLL_CTL_DEL,m_pSocketInfo->m_iSocket,&m_stEpollEvent) < 0)
        {
            LOG_ERROR("default","epoll remove socket error,socket fd = %d",m_pSocketInfo->m_iSocket);
        }
        //关闭socket
        closesocket(m_pSocketInfo->m_iSocket);
        //更改当前对大分配socket
        if(m_pSocketInfo->m_iSocket >= m_iMaxfds)
        {
            int iTmpUnUseSocket;
            for (iTmpUnUseSocket = m_pSocketInfo->m_iSocket -1 ;iTmpUnUseSocket >= m_iSocket;iTmpUnUseSocket--)
            {
                if(m_astSocketInfo[iTmpUnUseSocket].m_iSocketFlag != 0)
                {
                    break;
                }
                m_iMaxfds = iTmpUnUseSocket;
            }
        }
        //总连接数减一
        m_stTcpStat.m_iConnTotal --;
    }
    m_pSocketInfo->m_iSocket = INVALID_SOCKET;
    m_pSocketInfo->m_iSrcIP = 0;
    m_pSocketInfo->m_nSrcPort = 0;
    m_pSocketInfo->m_iDstIP= 0;
    m_pSocketInfo->m_nDstPort = short(-1);
    m_pSocketInfo->m_iRecvBytes = 0;
    m_pSocketInfo->m_iSocketType = 0;
    m_pSocketInfo->m_iSocketFlag = 0;
    m_pSocketInfo->m_tCreateTime = 0;
    m_pSocketInfo->m_tStamp = 0;
    m_pSocketInfo->m_iSendFlag = 0;
    m_pSocketInfo->m_iConnectedPort = 0;
}

/**
  * 函数名          : CGateCtrl::CheckWaitSendData
  * 功能描述        : 检测是否有缓存数据要下行客户端
  * 返回值          ：int
**/
int CGateCtrl::CheckWaitSendData()
{
    int iTmpRet = 0;
    int i = 0;
    int unTmpCodeLength = 0;

    //每次最多发送MAX_SEND_PKGS_ONCE个数据
    while(i < MAX_SEND_PKGS_ONCE)
    {
        //
        if( m_iSendIndex < m_S2CHead.socketinfos().size())
        {
            //有数据未发送，继续发送
            if (SendClientData())
            {
                i++;
                continue;
            }
        }
        else 
        {
            //当前没有接收到数据，先接收数据
            if (false == m_bHasRecv)
            {
                //没有可发送的数据或者发送完成,则接收gate数据
                iTmpRet = RecvServerData();
                //没有数据可接收，则发送队列无数据发送，退出
                if (iTmpRet == 0)
                {
                    break;
                }
                m_bHasRecv = true;
            }//处理已经接收到的数据
            else
            {
                //组织服务器发送到客户端的数据信息头，设置相关索引和游标
                m_iSendIndex = 0;
                m_iSCIndex = 0;
                m_nSCLength = 0;
                //反序列化消息的CTcpHead,取出发送游标和长度,把数据存入发送消息缓冲区m_szMsgBuf
                iTmpRet = ClientCommEngine::ConvertStreamToMessage(m_szSCMsgBuf,
                                                                unTmpCodeLength,
                                                                &m_S2CHead,
                                                                NULL,
                                                                NULL,
                                                                &m_iSendIndex);
                //序列化失败继续发送
                if(iTmpRet < 0)
                {
                    LOG_ERROR("default", "CTCPCtrl::CheckWaitSendData Error, ClientCommEngine::ConvertMsgToStream return %d.", iTmpRet);
                    m_S2CHead.Clear();
                    m_iSendIndex = 0;
                    continue;
                }

                //接收成功,取出数据长度
                char* pTmp = m_szSCMsgBuf;
                pTmp += m_iSCIndex;
                m_nSCLength = *(unsigned short*)pTmp;
            }
        }
    }
    return 0;
}

/**
  * 函数名          : CGateCtrl::DisConnect
  * 功能描述        : 通知gameserver客户端断开连接
  * 返回值          ：void
**/
void CGateCtrl::DisConnect(int iError)
{

    MesHead tmpHead;
    CSocketInfo* pSocketInfo = tmpHead.mutable_socketinfos()->Add();
    if (pSocketInfo == NULL)
    {
        LOG_ERROR("default","CTcpCtrl::DisConnect add_socketinfos ERROR");
        return;
    }
    pSocketInfo->set_socketid(m_pSocketInfo->m_iSocket);
    pSocketInfo->set_createtime(m_pSocketInfo->m_tCreateTime);
    pSocketInfo->set_state(iError);

    unsigned short unTmpMsgLen = (unsigned short) sizeof(m_szCSMsgBuf);

    int iRet = ClientCommEngine::ConvertToGameStream(m_szCSMsgBuf,unTmpMsgLen,&tmpHead);
    if (iRet != 0)
    {
        LOG_ERROR("default","[%s: %d : %s] ConvertMsgToStream failed,iRet = %d ",
                  __MY_FILE__,__LINE__,__FUNCTION__,iRet);
        return;
    }

    iRet = mC2SPipe->AppendOneCode((BYTE*)m_szCSMsgBuf,unTmpMsgLen);
    if (iRet != 0)
    {
        LOG_ERROR("default","[%s: %d : %s] Send data to GateServer failed,iRet = %d ",
                  __MY_FILE__,__LINE__,__FUNCTION__,iRet);
        return;
    }
    return;
}

/**
  * 函数名          : CGateCtrl::RecvServerData
  * 功能描述        : 接收gate返回的消息
  * 返回值          ：int 接收数据长度
**/
int CGateCtrl::RecvServerData()
{
    int unTmpCodeLength = MAX_PACKAGE_LEN;
    if (mS2CPipe->GetHeadCode((BYTE*)m_szSCMsgBuf,&unTmpCodeLength) < 0)
    {
        unTmpCodeLength = 0;
    }
    return unTmpCodeLength;
}

/**
  * 函数名          : CGateCtrl::SendClientData
  * 功能描述        : 向cliet发送数据
  * 返回值          ：int
**/
int CGateCtrl::SendClientData()
{
    BYTE*           pbTmpSend = NULL;
    unsigned short  unTmpShort;
    time_t          tTmpTimeStamp;
    int             iTmpSendBytes;
    int             nTmpIndex;
    unsigned short  unTmpPackLen;
    int             iTmpCloseFlag;

    auto* pSendList = m_S2CHead.mutable_socketinfos();
    //client socket索引非法，不存在要发送的client
    if (m_iSendIndex >= pSendList->size())
        return 0;
    CSocketInfo tmpSocketInfo = pSendList->Get(m_iSendIndex);

    //向后移动socket索引
    m_iSendIndex++;
    nTmpIndex = tmpSocketInfo.socketid();
    tTmpTimeStamp = tmpSocketInfo.createtime();

    //socket 非法
    if (nTmpIndex <=0 || MAX_SOCKET_NUM <= nTmpIndex)
    {
        LOG_ERROR("default","Invalid socket index %d",nTmpIndex);
        return -1;
    }

    /*
     * 时间不一样，说明这个socket是个新的连接，原来的连接已经关闭,注(原来的
     * 的连接断开后，新的客户端用了原来的socket fd ，因此数据不是现在这个连
     * 接的数据，原来连接的数据,中断发送
    */
    if (m_astSocketInfo[nTmpIndex].m_tCreateTime != tTmpTimeStamp)
    {
        LOG_ERROR("default","sokcet[%d] already closed(tcp createtime:%d:gate createtime:%d) : gate ==> client[%d] bytes \
                failed",nTmpIndex,m_astSocketInfo[nTmpIndex].m_tCreateTime,tTmpTimeStamp,m_nSCLength);
                return -1;
    }
    iTmpCloseFlag = tmpSocketInfo.state();
    unTmpPackLen = m_nSCLength;
    m_pSocketInfo = &m_astSocketInfo[nTmpIndex];
    //发送数据
    if (unTmpPackLen > 0)
    {
        //根据发送给客户端的数据在m_szSCMsgBuf中的数组下标取出数据
        pbTmpSend = (BYTE*)m_szSCMsgBuf[m_iSCIndex];
        memcpy((void*)&unTmpShort,(const void*)pbTmpSend,sizeof(unsigned short));
        if (unTmpShort != unTmpPackLen)
        {
            LOG_ERROR("default","Code length not matched,left length %u is less than body length %u",unTmpPackLen,unTmpShort);
            return -1;
        }
        iTmpSendBytes = TcpWrite(m_pSocketInfo->m_iSocket,(char*)pbTmpSend,unTmpPackLen);
        if (unTmpPackLen == iTmpSendBytes )
        {
            #ifdef _DEBUG_
                LOG_DEBUG("default","[us:%lu]TCP gate ==> client[%d][%s][%d Bytes]",GetUSTime(),nTmpIndex,\
                    m_astSocketInfo[nTmpIndex].m_szClientIP,iTmpSendBytes);
            #endif
            //统计信息
            m_stTcpStat.m_iPkgSend++;
            m_stTcpStat.m_iPkgSizeSend += iTmpSendBytes;
            //设置发送标志位已发送
            m_pSocketInfo->m_iSendFlag = MAIN_HAVE_SEND_DATA;
        }
        else
        {
            //发送失败
            ClearSocketInfo(Err_ClientClose);
            LOG_ERROR("default","send to client %s Failed due to error %d",m_pSocketInfo->m_szClientIP,errno);
            return 0;
        }
    }
    if (unTmpPackLen > 0 && iTmpCloseFlag == 0)
    {
        return 0;
    }

    if (iTmpCloseFlag == -1)
    {
        //
    }
    ClearSocketInfo(TCP_SUCCESS);
    return 0;
}
