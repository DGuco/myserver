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
#include "../inc/commdef.h"
#include "../../framework/net/client_comm_engine.h"
#include "../../framework/json/config.h"


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
    int iTmpRet;

    m_iLastKeepaliveTime = 0;
    m_iRunFlag = 0;
    m_iWriteStatCount = 0;
    m_SCTcpHead.Clear();
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

    //初始化epoll
    m_pEpollevents = NULL;
    //初始化epoll socket
    iTmpRet = InitEpollSocket((short)CServerConfig::GetSingletonPtr()->m_iTcpPort);
    if (0 != iTmpRet)
    {
        LOG_ERROR("default","InitEpollSocket failed! TCPserver init failed. ReusltCode = %d!",iTmpRet);
        return iTmpRet;   
    }
    LOG_INFO("default","InitEpollSocket successed! TCPserver init successed. ReusltCode = %d!",iTmpRet);

    m_astSocketInfo[m_iSocket].m_iSocket = m_iSocket;
    m_astSocketInfo[m_iSocket].m_iSocketType = LISTEN_SOCKET;
    m_astSocketInfo[m_iSocket].m_iSocketFlag = RECV_DATA;
    m_astSocketInfo[m_iSocket].m_iConnectedPort = CServerConfig::GetSingletonPtr()->m_iTcpPort;
    m_iMaxfds = m_iSocket++;

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
    }
    return 0;
}

int CTcpCtrl::SetRunFlag(int iRunFlag)
{
    m_iRunFlag = iRunFlag;
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
  * 函数名          : CTCPCtrl::EphInit
  * 功能描述        : 初始化Epoll event
  * 返回值          ：void 
**/
int CTcpCtrl::EphInit()
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
        LOG_ERROR("default","ERROR:%s",strerror(errno));
        LOG_ERROR("default","epoll create error!");
        close(m_iKdpfd);
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
    free(m_pEpollevents);
    close(m_iKdpfd);
    m_pEpollevents = NULL;
    return 0;
}

/**
  * 函数名          : CTCPCtrl::EphNewConn
  * 功能描述        : 创建连接
  * 返回值          ：void 
**/
int CTcpCtrl::EphNewConn(int iSocketFd)
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
  * 函数名          : CTCPCtrl::CheckTimeOut
  * 功能描述        : 检测客户端的连接超时和连接gate的超时
  * 返回值          ：void
**/
int CTcpCtrl::CheckTimeOut()
{

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
        m_pSocketInfo = &m_astSocketInfo[iTmpFd];
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
        
        if (m_pSocketInfo->m_iSocketType == LISTEN_SOCKET)
        {
            LOG_NOTICE("default","recv events:%d fd:%d",iTmpfdNum,iTmpFd);
            //accept 一个tcp连接
            iTmpNewSocket = accept(iTmpFd,(struct sockaddr*) &m_stSockAddr,(socklen_t*) &iTmpSocketAddrSize);
            //客户端主动关闭了连接
            if (iTmpNewSocket <= 0)
            {
                LOG_NOTICE("default","client canncled connection: port %d fd:%d,errno(%d : %s)",
                            m_pSocketInfo->m_iConnectedPort,iTmpNewSocket,errno,strerror(errno));
                continue;
            }

            m_stTcpStat.m_iConnIncoming ++;
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
            m_pSocketInfo = &m_astSocketInfo[iTmpFd];
            RecvClientData(iTmpFd);
        }

    }
    return 0;
}

/**
  * 函数名          : CTCPCtrl::RecvClientData
  * 功能描述        : 接客户端的数据组织客户端数据为服务器其间数据传格式存入消息包缓冲区
  * 返回值          ：void
**/
int CTcpCtrl::RecvClientData(int iSocketFd)
{
    int iTmpRet = 0;
    int iTmpRecvBytes = 0;
    int iTmpOffset;
    char* pTemp1;
    char* pTemp;
    unsigned int unRecvLen;
    unsigned int unLength = 0;
    int nRecvAllLen;
    time_t tTempTime;
    int iTmpSocket = m_pSocketInfo->m_iSocket;
    iTmpOffset = m_pSocketInfo->m_iRecvBytes; 
    
    //读取tcp数据
    iTmpRecvBytes = TcpRead(iTmpSocket,m_pSocketInfo->m_szMsgBuf + iTmpOffset,sizeof(m_pSocketInfo->m_szMsgBuf) - iTmpOffset); 
    //客户端关闭连接
    if (iTmpRecvBytes <= 0)
    {
        LOG_ERROR("default","Client[%s] close the tcp connection,socket id = %d,the client's port = ",
            m_pSocketInfo->m_szClientIP,m_pSocketInfo->m_iSocket,m_pSocketInfo->m_iConnectedPort);
        ClearSocketInfo(Err_ClientClose);
        return -1;
    }

    //统计接受信息
    m_stTcpStat.m_iPkgSizeRecv += iTmpRecvBytes;
    //增加收到的总字节数
    m_pSocketInfo->m_iRecvBytes = m_pSocketInfo->m_iRecvBytes + iTmpRecvBytes;
    //记录消息起始地址
    pTemp1 = m_pSocketInfo->m_szMsgBuf;
    nRecvAllLen = m_pSocketInfo->m_iRecvBytes;

    // 记录该socket接收客户端数据的时间
    time(&tTempTime);
    m_pSocketInfo->m_tStamp = tTempTime;

    while(1)
    {
        //小于最小长度继续接收
        if ( nRecvAllLen < MSG_HEAD_LEN)
        {
            LOG_ERROR("default","the package len is less than base len ,receive len %d",nRecvAllLen);
            break;
        }

        unLength = 0;
        //取出包的总长度
        memcpy(&unRecvLen,(void*)pTemp1,sizeof(unsigned int));
        unRecvLen = ntohl(unRecvLen) - MESSAGE_EXTRA_LEN;
        if (unRecvLen < MSG_HEAD_LEN || unRecvLen > MSG_MAX_LEN)
        {
            LOG_ERROR("default","the package len is illegal",nRecvAllLen);
            ClearSocketInfo(Err_PacketError);
            return -1;
        }
        //数据指针向后移动指向未读取位置
        pTemp1 += sizeof(unsigned int);
        nRecvAllLen -= sizeof(unsigned int);

        // 序列号长度
        unsigned short tTmpSeq = 0;
        memcpy(&tTmpSeq,(void*)pTemp1,sizeof(unsigned short));
        tTmpSeq = ntohs(tTmpSeq);
        pTemp1 += sizeof(unsigned short);
        nRecvAllLen -= sizeof(unsigned short);

        // protobuf版本
        unsigned char tTmpProbufVersion = 0;
        memcpy(&tTmpProbufVersion,(void*)pTemp1,sizeof(unsigned char));
        pTemp1 += sizeof(unsigned char);
        nRecvAllLen -= sizeof(unsigned char);

        // 是否加密
        unsigned char tTmpIsEncry = 0;
        memcpy(&tTmpIsEncry,(void*)pTemp1,sizeof(unsigned char));
        pTemp1 += sizeof(unsigned char);
        nRecvAllLen -= sizeof(unsigned char);

        //消息指令编号
        unsigned short tTmpCmd = 0;
        memcpy(&tTmpCmd,(void*)pTemp1,sizeof(unsigned short));
        tTmpCmd = ntohs(tTmpCmd);
        pTemp1 += sizeof(unsigned short);
        nRecvAllLen -= sizeof(unsigned short);

        nRecvAllLen -= unRecvLen;
        pTemp1 += unRecvLen;
        // 总长度小于包的长度，则继续接收
        if(nRecvAllLen < 0)
        {
            nRecvAllLen = m_pSocketInfo->m_iRecvBytes;
            pTemp1      = m_pSocketInfo->m_szMsgBuf;
            LOG_DEBUG("default", "Receive client part data left len = %d",nRecvAllLen,unRecvLen);
            break;
        }

        CMessage tmpMsg;
        tmpMsg.Clear();
        CMessageHead* pbMessageHead = tmpMsg.mutable_msghead();
        pbMessageHead->set_messageid(tTmpCmd);
        pbMessageHead->set_dstfe(FE_GATESERVER);
        pbMessageHead->set_dstid(20000);
        pbMessageHead->set_timestamp(tTempTime);
        iTmpRet = ClientCommEngine::ConvertClientStreamToMsg(pTemp1 - unRecvLen,unRecvLen,&tmpMsg);
        if (iTmpRet != 0)
        {
            LOG_ERROR("default","CTCPCtrl::RecvClientData error,ConvertStreamTomsg return %d",iTmpRet);
            ClearSocketInfo(Err_PacketError);
            return -1;
        }
        
        //组织转发消息
        if (tmpMsg.msghead().messageid() != CMsgPingRequest::MsgID)
        {
            CTcpHead pbTmpTcpHead;
            pbTmpTcpHead.Clear();
            pbTmpTcpHead.set_srcfe(FE_TCPSERVER);      //设置源服务器
            pbTmpTcpHead.set_srcid(CServerConfig::GetSingletonPtr()->m_iTcpServerId);
            pbTmpTcpHead.set_dstfe(tmpMsg.msghead().dstfe());
            pbTmpTcpHead.set_srcid(tmpMsg.msghead().dstid());
            pbTmpTcpHead.set_timestamp(tTempTime);
            CSocketInfo* pbSocketInfo = pbTmpTcpHead.add_socketinfos();
            if (pbSocketInfo)
            {
                LOG_ERROR("default","CTCPCtrl::RecvClientData error,add socketingo error,pbSocketInfo is NULL");
                ClearSocketInfo(Err_PacketError);
            }

            pbSocketInfo->set_createtime(m_pSocketInfo->m_tCreateTime);
            pbSocketInfo->set_socketid(iSocketFd);
            pbSocketInfo->set_createtime(m_pSocketInfo->m_tCreateTime);
            pbSocketInfo->set_srcip(m_pSocketInfo->m_iSrcIP);
            pbSocketInfo->set_srcport(m_pSocketInfo->m_nSrcPort);
            //state < 0 说明关闭socket
            pbSocketInfo->set_state(0);

            pTemp = m_szMsgBuf;
            //预留总长度
            pTemp += sizeof(short);
            unLength += sizeof(short);
            //预留8字节对齐长度
            pTemp += sizeof(short);
            unLength += sizeof(short);
            //序列化CTCPhead
            *(short*)pTemp = pbTmpTcpHead.ByteSize();
            pTemp += sizeof(short);
            unLength += sizeof(short);

            //序列化CTCPhead
            if (pbTmpTcpHead.SerializeToArray(pTemp, sizeof(m_szMsgBuf) - unLength - 1) != true)
            {
                LOG_ERROR("default", "CTCPCtrl::RecvClientData error,pbTmpTcpHead SerializeToArray error");
                ClearSocketInfo(Err_PacketError);
                return -1;
            }

            pTemp += pbTmpTcpHead.GetCachedSize();
            unLength += pbTmpTcpHead.GetCachedSize();

            //拷贝消息到发送缓冲区
            memcpy(pTemp,pTemp1,unRecvLen);
            pTemp += unRecvLen;
            unLength += unRecvLen;

            //8字节对齐
            unsigned short iTmpAddlen = (unLength % 8);
            if (iTmpAddlen > 0)
            {
                iTmpAddlen = 8 - iTmpAddlen;
                //将字节对齐部分置为0
                memset(pTemp,0,iTmpAddlen);
            }

            pTemp += iTmpAddlen;
            unLength += iTmpAddlen;
            //序列话消息总长度
            pTemp = m_szMsgBuf;
            *(short*) pTemp = unLength;
            pTemp += sizeof(short);

            //序列话8字节对齐长度
            *(short*) pTemp = iTmpAddlen;
            iTmpRet = m_GateClient.SendOneCode(unLength,(unsigned char*) m_szMsgBuf);
            if (iTmpRet < 0)
            {
                LOG_ERROR("default", "CTCPCtrl::RecvClientData error,send data to gate error,error code = %d",iTmpRet);
                ClearSocketInfo(Err_PacketError);
                return iTmpRet;
            }
            #ifdef _DEBUG_
                LOG_DEBUG("defalut","tcp ==>gate [%d bytes]",unLength);
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
        //继续转发
        m_pSocketInfo->m_iRecvBytes = nRecvAllLen;
        memmove(m_pSocketInfo->m_szMsgBuf,pTemp1,nRecvAllLen);
    }
    return 0;
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
    if ( TCP_SUCCESS == enError)
    {

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
  * 函数名          : CTCPCtrl::CheckWaitSendData
  * 功能描述        : 检测是否有缓存数据要发送
  * 返回值          ：int
**/
int CTcpCtrl::CheckWaitSendData()
{
    return 0;
}

/**
  * 函数名          : CTCPCtrl::ConnectToGate
  * 功能描述        : 连接gate服务器
  * 返回值          ：int
**/
bool CTcpCtrl::ConnectToGate()
{

}

/**
  * 函数名          : CTCPCtrl::RegisterToGate
  * 功能描述        : 注册gate服务器
  * 返回值          ：bool
**/
bool CTcpCtrl::RegisterToGate()
{

}

/**
  * 函数名          : CTCPCtrl::RegisterToGate
  * 功能描述        : 向gate服务器发送心跳信息
  * 返回值          ：bool
**/
bool CTcpCtrl::SendKeepAliveToGate()
{

}

/**
  * 函数名          : CTCPCtrl::DisConnect
  * 功能描述        : 通知gameserver客户端断开连接
  * 返回值          ：void
**/
void CTcpCtrl::DisConnect(int iError)
{

}

/**
  * 函数名          : CTCPCtrl::RecvServerData
  * 功能描述        : 接收gate返回的消息
  * 返回值          ：int
**/
int CTcpCtrl::RecvServerData()
{

}