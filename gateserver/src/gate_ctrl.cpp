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

extern CRunFlag g_byRunFlag;

#ifdef _POSIX_MT_
	pthread_mutex_t CGateCtrl::stLinkMutex[EHandleType_NUM];
	pthread_mutex_t CGateCtrl::stMutex[MUTEX_NUM];
#endif

CGateCtrl::CGateCtrl()
{
	m_mapConns.clear();

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

	return tpConn;
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

int CGateCtrl::CheckConnRequest()
{


	return 1;
}

/********************************************************
  Function:     ReceiveAndProcessRegister
  Description:  接收并处理注册消息
  Input:          iUnRegisterIdx:  未注册结构索引
  Output:      
  Return:       0 :   成功 ，其他失败
  Others:		
********************************************************/
int CGateCtrl::ReceiveAndProcessRegister(int iUnRegisterIdx)
{
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
