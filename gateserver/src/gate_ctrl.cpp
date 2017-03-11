//
//  gate_ctrl.cpp
//  gate_ctrl 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//
#include "../inc/gate_ctrl.h"
#include "../../framework/net/runflag.h"
#include "../../framework/log/log.h"
#include "../../framework/message/message.pb.h"
#include "../../framework/message/tcpmessage.pb.h"
#include "../../framework/net/client_comm_engine.h"
#include "../../framework/message/proxymessage.pb.h"

extern CRunFlag g_byRunFlag;

#ifdef _POSIX_MT_
	pthread_mutex_t CGateCtrl::stLinkMutex[EHandleType_NUM];
	pthread_mutex_t CGateCtrl::stMutex[MUTEX_NUM];
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

CMyTCPConn* CGateCtrl::GetConnByKey(int iKey)
{
	CMyTCPConn* tcpConn = NULL;
#ifdef _POSIX_MT_
	pthread_mutex_lock(&stMutex[MUTEX_HASHMAP]);
#endif

	auto iter = m_mapConns.find(iKey);
	if (iter != m_mapConns.end())
	{
		tcpConn = iter->second;
	}

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&stMutex[MUTEX_HASHMAP]);
#endif

	return tcpConn;
}


int CGateCtrl::MakeConnKey(const short nType, const short nID)
{

	return iKey;
}


// 将连接插入handle并唤醒handle
int CGateCtrl::WakeUp2Work(CMyTCPConn* pConn)
{


	return iRet;
}


// 获取一个可用连接
CMyTCPConn* CGateCtrl::GetCanUseConn()
{

	return pConn;
}


// 回收一个连接
CMyTCPConn* CGateCtrl::RecycleUnuseConn(CMyTCPConn* pConn, int iIndex)
{

	return tpNext;
}


// 将连接插入map
int CGateCtrl::InsertConnIntoMap(CMyTCPConn* pConn, int iIndex)
{

	return 0;
}


// 将连接从map中移除
int CGateCtrl::EraseConnFromMap(CMyTCPConn* pConn, int iIndex)
{

	return 0;
}


int CGateCtrl::CheckRunFlags()
{
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

	//等待读取
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
	CProxyHead stTmpProsyHead;
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

	char* pcTmpData = acTmpBuf;
	//接收总长度
	unsigned short iPkgSize = *((unsigned short*)(pcTmpData));
	pcTmpData += sizeof(unsigned short);
	if (iPkgSize != iRecvedBytes)
	{
		LOG_ERROR("default","[%s:%d:%s] iPkgSize = %d,iRecvedBytes = %d",__MY_FILE__,__LINE__,__FUNCTION__,
			iPkgSize,iRecvedBytes);
		close(iSocketFd);
		return -1;
	}
	//8字节对齐长度舍弃
	pcTmpData += 2;
	//CProxyHeadSize长度
	unsigned short iTmpProxyPkgSize = *((unsigned short*)(acTmpBuf));
	pcTmpData += sizeof(unsigned short);
	//获取proxyhead
	if(stTmpProsyHead.ParseFromArray(acTmpBuf,iTmpProxyPkgSize) == false)
	{
		LOG_ERROR("default","stTmpProsyHead ParseFromArray len = %d",iTmpProxyPkgSize);
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
int CGateCtrl::CheckRoutines() {
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
		m_stHandleInfos[i].mpHandle->Initialize((EMHandleType)i,&(m_stHandleInfos->mLinkerInfo),&m_mapConns);
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
