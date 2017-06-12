//
//  gate_ctrl.cpp
//  gate_ctrl 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//
#include "../inc/gate_ctrl.h"
#include "../../framework/net/runflag.h"
#include "../../framework/message/message.pb.h"
#include "../../framework/message/tcpmessage.pb.h"
#include "../../framework/net/client_comm_engine.h"

extern CRunFlag g_byRunFlag;

#ifdef _POSIX_MT_
	std::mutex CGateCtrl::stLinkMutex[EHandleType_NUM];
	std::mutex CGateCtrl::stMutex[MUTEX_NUM];
#endif

CGateCtrl::CGateCtrl()
{
	m_mapConns.clear();
	//把连接数组元素插入空闲连接双向链表中
	for (int i = 0; i < MAX_CONNS_NUM;i++)
	{
		m_UnuseConns.insert(&m_astConns[i]);
	}

    m_iCurrentUnRegisterNum = 0;
	memset((void*)m_astUnRegisterInfo,0,sizeof(m_astUnRegisterInfo));
	time(&m_tLastCheckTime);
}

CGateCtrl::~CGateCtrl()
{

}

int CGateCtrl::Initialize()
{
	return 0;
}

/**
  *函数名          : GetConnByKey
  *功能描述        : 通过key获取一个连接信息
**/
CMyTCPConn* CGateCtrl::GetConnByKey(int iKey)
{
	CMyTCPConn* tcpConn = NULL;
#ifdef _POSIX_MT_
	std::lock_guard<std::mutex> guard(stMutex[MUTEX_HASHMAP]);
#endif

	auto iter = m_mapConns.find(iKey);
	if (iter != m_mapConns.end())
	{
		tcpConn = iter->second;
	}
	return tcpConn;
}

/**
  *函数名          : MakeConnKey
  *功能描述        : 生成key
**/
int CGateCtrl::MakeConnKey(const short nType, const short nID)
{
	int iKey = 0;
	iKey = nType;
	//高16位保存服务器类型，低16位保存服务器id
	iKey = (iKey << 16) | nID;
	return iKey;
}

/**
  *函数名          : WakeUp2Work
  *功能描述        : 将新的连接插入handle并唤醒handle
**/
int CGateCtrl::WakeUp2Work(CMyTCPConn* pConn)
{
    int iIndex = 0;
    int minNum = m_stHandleInfos[0].miConnNum;
    //找出handle连接数最好的handle
    for (int i = 1; i < EHandleType_NUM;i++)
    {
        if (minNum > m_stHandleInfos->miConnNum)
        {
            minNum = m_stHandleInfos->miConnNum;
            iIndex = i;
        }
    }
    //插入连接map
    int iRet = InsertConnIntoMap(pConn,iIndex);
    if (iRet != 0)
    {
        return iRet;
    }

    //唤醒处理线程
    LOG_DEBUG("default", "Wake up handle(%d) start to work.", iIndex);
    pConn->SetConnState(ENTITY_ON);
    pConn->SetLastKeepalive(time(NULL));
    m_stHandleInfos[iIndex].mpHandle->WakeUp();
    return iRet;
}

/**
  *函数名          : GetCanUseConn
  *功能描述        : 获取一个可用连接
**/
CMyTCPConn* CGateCtrl::GetCanUseConn()
{
	//获取可用连接链表的头
	CMyTCPConn* pConn = (CMyTCPConn*) m_UnuseConns.GetHead();
	//没有可用连接
	if (pConn == NULL)
	{
		LOG_WARN("default","No unuse conns.");
		return NULL;
	}

	//从可用链表中删除
	m_UnuseConns.erase(pConn);
	return pConn;
}

/**
  *函数名          : RecycleUnuseConn
  *功能描述        : 回收一个连接
**/
CMyTCPConn* CGateCtrl::RecycleUnuseConn(CMyTCPConn* pConn, int iIndex)
{
    CMyTCPConn* pNext = NULL;
    if (pConn == NULL)
    {
        return NULL;
    }
    pNext =  (CMyTCPConn*)pConn->GetNext();
    if (iIndex > 0 && iIndex < EHandleType_NUM)
    {
        //从连接map中移除
        EraseConnFromMap(pConn,iIndex);
    }
    //回收该连接
    m_UnuseConns.insert(pConn);
	return pNext;
}

/**
  *函数名          : InsertConnIntoMap
  *功能描述        : 将连接插入map
**/
int CGateCtrl::InsertConnIntoMap(CMyTCPConn* pConn, int iIndex)
{
    if (pConn == NULL || iIndex < 0 || iIndex >= EHandleType_NUM )
    {
        return -1;
    }

    //生成key
    int iKey = MakeConnKey(pConn->GetEntityType(),pConn->GetEntityID());
#ifdef _POSIX_MT_
    std::lock_guard<std::mutex> guard(stMutex[MUTEX_HASHMAP]);
#endif
    //超过最大连接数
    if (m_mapConns.size() > MAX_CONNS_NUM)
    {
        LOG_ERROR("default", "insert conn(key=%d) into m_mapConns failed.", iKey);
        return -2;
    }
    else
    {
        m_mapConns.insert(std::make_pair(iKey,pConn));
        m_stHandleInfos[iIndex].miConnNum++;
#ifdef _POSIX_MT_
        std::lock_guard<std::mutex> guard(stLinkMutex[iIndex]);
#endif
        m_stHandleInfos[iIndex].mLinkerInfo.insert(pConn);
        LOG_INFO("default", "insert conn(key=%d) into m_mapConns succeed.", iKey);
    }

	return 0;
}

/**
  *函数名          : EraseConnFromMap
  *功能描述        : 将连接从map中移除
**/
int CGateCtrl::EraseConnFromMap(CMyTCPConn* pConn, int iIndex)
{
    if (pConn == NULL || iIndex < 0 || iIndex >= EHandleType_NUM )
    {
        return -1;
    }

    //生成key
    int iKey = MakeConnKey(pConn->GetEntityType(),pConn->GetEntityID());
#ifdef _POSIX_MT_
    std::lock_guard<std::mutex> guard(stMutex[MUTEX_HASHMAP]);
#endif
    m_mapConns.erase(iKey);
    LOG_INFO("default", "erase conn(key=%d) from m_mapConns.", iKey);
    m_stHandleInfos[iIndex].miConnNum--;
    m_stHandleInfos[iIndex].mLinkerInfo.erase(pConn);
    return 0;
}


int CGateCtrl::CheckRunFlags()
{
    //暂不支持ERF_RELOAD
    if (g_byRunFlag.CheckRunFlag(ERF_RELOAD))
    {
        g_byRunFlag.SetRunFlag(ERF_RUNTIME);
    }
	return 0;
}

/********************************************************
  Function:     CheckConnRequest
  Description:  接收连接请求
  Input:
  Output:
  Return:       0 :   成功 ，其他失败
  Others:
********************************************************/
int CGateCtrl::CheckConnRequest()
{
	fd_set fds_read;
	timeval tcTimpListen;
	int i = 0,iTmp;
	int iListenSocketFd = -1;
	int iNewSocketFd = -1;
	struct sockaddr_in stTmpConnAddr;
	TFName szTmpConnAddr;
	int iMaxSocketFd = -1;

	memset(&stTmpConnAddr,0,sizeof(stTmpConnAddr));
	socklen_t iTmpAddrLength = sizeof(stTmpConnAddr);

	//等待10毫秒超时
	tcTimpListen.tv_sec = 0;
	tcTimpListen.tv_usec = 10000;
	FD_ZERO(&fds_read);
	iListenSocketFd = m_stListenSocket.GetSocketFD();
	// 获取监听socketfd失败
	if (iListenSocketFd < 0)
	{
		LOG_ERROR("default","Get listen socket error,listen fd = %d",iListenSocketFd);
		return -1;
	}
	//将监听端口加入端口集
	FD_SET(iListenSocketFd,&fds_read);
	iMaxSocketFd = iListenSocketFd;

	//将已经建立连接的socket加入端口集，select检测是否有数据可读
	for (int i = 0;i < m_iCurrentUnRegisterNum; i++)
	{
		if (m_astUnRegisterInfo[i].m_iRegisted == 0)
		{
			FD_SET(m_astUnRegisterInfo[i].m_iSocketFD,&fds_read);
			if(m_astUnRegisterInfo[i].m_iSocketFD > iMaxSocketFd)
			{
				iMaxSocketFd = m_astUnRegisterInfo[i].m_iSocketFD;
			}
		}
	}

	/*
	 * 等待读取 注：select返回可读socket数量同时会把不可读的socket从端口集中清除掉
	 * 因此后面再次FD_SET判断，如果仍然在端口集中socket可读
	 */
	iTmp = select(iMaxSocketFd + 1,&fds_read,NULL,NULL,&tcTimpListen);

	//没有可读取返回
	if (iTmp <= 0)
	{
		if (iTmp < 0)
		{
			LOG_ERROR("default","Select error,%s",strerror(errno));
		}
		return iTmp;
	}

	if (FD_ISSET(iListenSocketFd,&fds_read))
	{
		//接收这个连接
		iNewSocketFd = accept(iListenSocketFd,(struct sockaddr*)&stTmpConnAddr,&iTmpAddrLength);
		SockAddrToString(&stTmpConnAddr,(char*)szTmpConnAddr);
		LOG_INFO("default","Get a conn request from %s socket fd = %d",szTmpConnAddr,iNewSocketFd);
		if (m_iCurrentUnRegisterNum >= MAX_UNREGISTER_NUM)
		{
			LOG_ERROR("default","Error there is no empty space(curr num:%d),to record",m_iCurrentUnRegisterNum);
			//关闭socket
			close(iNewSocketFd);
		}
		else
		{
			m_astUnRegisterInfo[m_iCurrentUnRegisterNum].m_iSocketFD = iNewSocketFd;
			m_astUnRegisterInfo[m_iCurrentUnRegisterNum].m_ulIPAddr = stTmpConnAddr.sin_addr.s_addr;
			m_astUnRegisterInfo[m_iCurrentUnRegisterNum].m_tAcceptTime = GetMSTime();
            //未注册索引加一
			m_iCurrentUnRegisterNum++;

			int iTmpOptLen = sizeof(socklen_t);
			int iOptVal = SENDBUFSIZE;
            setsockopt(iNewSocketFd,SOL_SOCKET,SO_SNDBUF,(const void*)&iOptVal,iTmpOptLen);
            if (getsockopt(iNewSocketFd,SOL_SOCKET,SO_SNDBUF,(void*)&iOptVal,(socklen_t*)&iTmpOptLen) == 0)
            {
                LOG_ERROR("default", "socket %d set send bufflen = %d error", iNewSocketFd,iOptVal);
            }
		}
	}

    //从后往前遍历，因为在ReceiveAndProcessRegister中会删除
    for (i = m_iCurrentUnRegisterNum - 1;i >= 0;i++)
    {
		//socket是否仍然在端口集中，如果在读取数据并处理
        if (FD_ISSET(m_astUnRegisterInfo[i].m_iSocketFD,&fds_read))
        {
            //注册连接
            ReceiveAndProcessRegister(i);
        }
    }
	return 1;
}

/********************************************************
  Function:     ReceiveAndProcessRegister
  Description:  接收并处理注册消息
  Input:        iUnRegisterIdx:  未注册结构索引
  Output:      
  Return:       0 :   成功 ，其他失败
  Others:		
********************************************************/
int CGateCtrl::ReceiveAndProcessRegister(int iUnRegisterIdx)
{
	char acTmpBuf[MAX_TMP_BUF_LEN] = {0};
	int iRecvedBytes = 0;
	CTcpHead stTmpTcpHead;
	CMyTCPConn* pAcceptConn = NULL;

	//索引非法
	if (iUnRegisterIdx < 0 || iUnRegisterIdx >= m_iCurrentUnRegisterNum)
	{
		LOG_ERROR("default","Error in ReceiveAndProcessRegister,Unregister idx(%d) is invalid",iUnRegisterIdx);
		return -1;
	}

	int iSocketFd = m_astUnRegisterInfo[iUnRegisterIdx].m_iSocketFD;
	u_long ulIPAddr = m_astUnRegisterInfo[iUnRegisterIdx].m_ulIPAddr;

	//接收数据
	iRecvedBytes = recv(iSocketFd,acTmpBuf,sizeof(acTmpBuf),0);
	//如果连接中断
	if (iRecvedBytes == 0)
	{
		LOG_ERROR("default","The remote site may closed this connect fd = %d,errno = %s",iSocketFd,errno);
		DeleteOneUnRegister(iUnRegisterIdx);
		close(iSocketFd);
		return -1;
	}

	//如果出错
	if (iRecvedBytes < 0)
	{
		//如果不是无数据可读
		if (errno != EAGAIN)
		{
			LOG_ERROR("default","Error in read conn fd = %d,errno = %s",iSocketFd,errno);
			DeleteOneUnRegister(iUnRegisterIdx);
			close(iSocketFd);
		}
		return -1;
	}

	//删除相应的索引
	DeleteOneUnRegister(iUnRegisterIdx);

	stTmpTcpHead.Clear();
	unsigned short unOffset = 0;
	//获取tcphead
	int iRet = ClientCommEngine::ConvertStreamToMsg(acTmpBuf,iRecvedBytes,unOffset,&stTmpTcpHead);
	if (iRet < 0)
	{
		LOG_ERROR("default","[%s : %d : %s] ConvertStreamToMsg error,errno = %d",
			__MY_FILE__,__LINE__,__FUNCTION__,iRet);
		return -1;
	}

	//判断是否是注册消息
	if (stTmpTcpHead.dstfe () != FE_GATESERVER \
		|| stTmpTcpHead.dstid() != CServerConfig::GetSingletonPtr()->m_iGateServerId \
		|| stTmpTcpHead.opflag() != EGC_REGIST)
	{
		LOG_ERROR("default","Error CCSHead is invalid,fd = %d,Src(FE = %d : ID = %d),Dst(FE = %d : ID = %d),OpFlag = %d,TimeStamp = %ld.",
			iSocketFd,stTmpTcpHead.srcfe(),stTmpTcpHead.srcid(),
			stTmpTcpHead.srcfe(),stTmpTcpHead.srcid(),
			stTmpTcpHead.opflag(),stTmpTcpHead.timestamp());
		close(iSocketFd);
		return -1;
	}

#ifdef _DEBUG_
	LOG_DEBUG("default","************** Recv Msg ****************");
	LOG_DEBUG("default","[%s]",stTmpTcpHead.ShortDebugString().c_str());
#endif

	//检查该链接是否已存在
	int iKey = MakeConnKey(stTmpTcpHead.srcfe(),stTmpTcpHead.srcid());
	pAcceptConn = GetConnByKey(iKey);
	if (pAcceptConn != NULL)
	{
		LOG_DEBUG("default","The Conn has existed FE = % : id = %d : key = %d",stTmpTcpHead.srcfe(),stTmpTcpHead.srcid());
		close(iSocketFd);
		return -1;
	}

	//生成一个可用连接
	pAcceptConn = GetCanUseConn();
	if (pAcceptConn == NULL)
	{
		//没有可用连接
        close(iSocketFd);
        return -1;
	}
    pAcceptConn->Initialize(stTmpTcpHead.srcfe(),stTmpTcpHead.srcid(),ulIPAddr,0);

    // accept该连接并改为ENTITY_OFF状态
    int iAcceptRst = pAcceptConn->EstConn(iSocketFd);
    if (iAcceptRst < 0)
    {
        if (iAcceptRst == -2)
        {
            LOG_ERROR("default", "The server had connected, ignore the conn request (fd: %d).", iSocketFd);
        }
        // 回收连接
        RecycleUnuseConn(pAcceptConn);
        close(iSocketFd);
        return -1;
    }

    LOG_INFO("default", "Conn(FE = %d, ID = %d, KEY = %d) connected OK, the FD is %d.",
             pAcceptConn->GetEntityType(), pAcceptConn->GetEntityID(), iKey, iSocketFd);

    // 分配线程并唤醒
    iAcceptRst = WakeUp2Work(pAcceptConn);
    if (iAcceptRst != 0)
    {
        LOG_ERROR("default", "WakeUp2Work failed, conn(FE = %D, ID = %d, KEY = %d) FD is %d.",
                  pAcceptConn->GetEntityType(), pAcceptConn->GetEntityID(), iKey, iSocketFd);
        // 回收连接
        RecycleUnuseConn(pAcceptConn);
        close(iSocketFd);
        return -1;
    }

    return 0;
}

/********************************************************
  Function:     DeleteOneUnRegister
  Description:  删除一个未注册的连接
  Input:          iUnRegisterIdx:  未注册结构索引
  Output:      
  Return:       0 :   成功 ，其他失败
  Others:		
********************************************************/
int CGateCtrl::DeleteOneUnRegister(int iUnRegisterIdx)
{
    if (iUnRegisterIdx < 0 || iUnRegisterIdx >= m_iCurrentUnRegisterNum)
    {
        LOG_ERROR("default","Error in DeleteOneUnRegister,Unregister idx(%d) is invalid",iUnRegisterIdx);
        return -1;
    }

    m_iCurrentUnRegisterNum--;
    if ((m_iCurrentUnRegisterNum > 0) || (iUnRegisterIdx < m_iCurrentUnRegisterNum))
    {
        m_astUnRegisterInfo[iUnRegisterIdx] = m_astUnRegisterInfo[m_iCurrentUnRegisterNum];
        m_astUnRegisterInfo[m_iCurrentUnRegisterNum].Clear();
    }

	return 0;
}

/********************************************************
  Function:     CheckRoutines
  Description:  定时检查
  Input:         
  Output:      
  Return:       0 :   成功 ，其他失败
  Others:		
********************************************************/
int CGateCtrl::CheckRoutines()
{
    time_t tCurrentTime = time(NULL);
    if (tCurrentTime - m_tLastCheckTime >= CHECK_INTERVAL_SECONDS)
    {
        m_tLastCheckTime = tCurrentTime;
        for (int i = m_iCurrentUnRegisterNum; i >= 0;i--)
        {
            //检测连接是否超时
            if (tCurrentTime - m_astUnRegisterInfo[i].m_tAcceptTime > CHECK_TIMEOUT_SECONDS)
            {
                int iSocketFd = m_astUnRegisterInfo->m_iSocketFD;
                LOG_ERROR("default", "Wait register timeout so close it, fd = %d.", iSocketFd);
                DeleteOneUnRegister(i);
                close(iSocketFd);
            }
        }

        // 检查各handle中是否有已关闭的连接，并回收
        for (int i = 0; i < EHandleType_NUM; i++)
        {
#ifdef _POSIX_MT_
            std::lock_guard<std::mutex> guard(stLinkMutex[i]);
#endif
            CMyTCPConn* tpConn = (CMyTCPConn*) m_stHandleInfos[i].mLinkerInfo.GetHead();
            while (tpConn != NULL)
            {
                if (tpConn->IsStateOn() == false || (tCurrentTime - tpConn->GetLastKeepalive() >= CHECK_TIMEOUT_SECONDS))
                {
                    // 连接已改为关闭状态或者keepalive超时
                    tpConn = RecycleUnuseConn(tpConn, i);
                }
                else
                {
                    tpConn = (CMyTCPConn*) tpConn->GetNext();
                }
            }
        }
    }

    return 0;
}

int CGateCtrl::PrepareToRun()
{
	int i;
	//监听socket
	if(m_stListenSocket.CreateServer(CServerConfig::GetSingletonPtr()->m_iGatePort))
	{
		return -1;
	}

	for (i = 0; i < EHandleType_NUM;i++)
	{
		m_stHandleInfos[i].mpHandle = new CGateHandle;
		//如果创建失败退出
		if(m_stHandleInfos[i].mpHandle == NULL)
		{
			return -1;
		}
		//初始化线程
		m_stHandleInfos[i].mpHandle->Initialize((EMHandleType)i,&(m_stHandleInfos[i].mLinkerInfo),&m_mapConns);
		//创建线程
		if (m_stHandleInfos[i].mpHandle->CreateThread())
		{
			return -1;
		}
	}
	return 0;
}

int CGateCtrl::Run()
{

	while (True)
	{
		CheckRunFlags();  // 检查是否设置了flags

		CheckConnRequest();  // 检查是否有连接请求，如果有则连接并处理注册信息

		CheckRoutines();  // 检查这些注册请求是否超时，超时则关闭
	}	//end while

	return 0;
}
