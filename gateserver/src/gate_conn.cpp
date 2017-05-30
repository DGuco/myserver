//
//  gate_conn.cpp
//  gate_conn 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include "../inc/gate_conn.h"

/**
  *函数名          : EstConn
  *功能描述        : 建立连接
**/
int CMyTCPConn::EstConn(int iAcceptFD)
{
#ifdef _POSIX_MT_
	std::lock_guard<std::mutex> guard(m_stMutex);
#endif
	int iTempRet = 0;
	iTempRet = GetSocket()->Accept(iAcceptFD);
	m_iConnState = ENTITY_OFF;
	return iTempRet;
}

/**
  *函数名          : IsConnCanRecv
  *功能描述        : 检测连接是否准备好接受数据
**/
int CMyTCPConn::IsConnCanRecv()
{
#ifdef _POSIX_MT_
	std::lock_guard<std::mutex> guard(m_stMutex);
#endif
    if (GetSocket()->GetSocketFD() < 0 && GetSocket()->GetStatus() == tcs_connected)
    {
        return  True;
    }
    else
    {
        return False;
    }
}
/**
  *函数名          : RegToCheckSet
  *功能描述        : 把socket添加到pCheckSet集合中
**/
int CMyTCPConn::RegToCheckSet(fd_set *pCheckSet) {
    int iTmpRet = 0;
#ifdef _POSIX_MT_
    std::lock_guard<std::mutex> guard(m_stMutex);
#endif
    iTmpRet = GetSocket()->AddToCheckSet(pCheckSet);
	return iTmpRet;
}

/**
  *函数名          : IsFDSetted
  *功能描述        : 检测socket是否添加到pCheckSet集合中
**/
int CMyTCPConn::IsFDSetted(fd_set *pCheckSet) {
	int iTmpRet = 0;
#ifdef _POSIX_MT_
	std::lock_guard<std::mutex> guard(m_stMutex);
#endif
	// 查看该套接字的FD是否在套接字集合中
	iTmpRet = GetSocket()->IsFDSetted(pCheckSet);
	return iTmpRet;
}

int CMyTCPConn::RecvAllData()
{
	int iTmpRet = 0;
#ifdef _POSIX_MT_
	std::lock_guard<std::mutex> guard(m_stMutex);
#endif
	iTmpRet = GetSocket()->RecvData();
	return iTmpRet;
}

/**
  *函数名          : RecvAllData
  *功能描述        : 接受所有数据
**/
int CMyTCPConn::RecvAllData()
{
#ifdef _POSIX_MT_
    std::lock_guard<std::mutex> guard(m_stMutex);
#endif
    return GetSocket()->RecvData();
}

/**
  *函数名          : RecvAllData
  *功能描述        : 接受指定长度的数据
**/
int CMyTCPConn::GetOneCode(short &nCodeLength, BYTE *pCode)
{
#ifdef _POSIX_MT_
    std::lock_guard<std::mutex> guard(m_stMutex);
#endif
	return GetSocket()->GetOneCode((unsigned short int &)nCodeLength, pCode);
}

int CMyTCPConn::SendCode(short nCodeLength, BYTE *pCode, int iFlag /* = FLG_CONN_IGNORE */)
{
	int iTmpRet = 0;
#ifdef _POSIX_MT_
	std::lock_guard<std::mutex> guard(m_stMutex);
#endif
    //无效的socket连接，直接返回
    if (GetEntityID() < 0 || GetEntityID() < 0)
	{
		return -101;
	}

    // 外带数据和只读操作优先发送, 不参与排队
    if (iFlag == FLG_CONN_CTRL || iFlag == FLG_CONN_IGNORE)
    {
        if (GetSocket()->GetStatus() == tcs_connected && GetSocket()->GetSocketFD() > 0)
        {
            iTmpRet = GetSocket()->SendOneCode(nCodeLength,pCode);
        }
        else
        {
            iTmpRet = -102;
        }
    }
	return iTmpRet;
}

int CMyTCPConn::CleanBlockQueue(int iQueueLength) {
	//return iRedoCount;
	return 0;
}

int CMyTCPConn::SetConnState(int iConnState) {

	return 0;
}

bool CMyTCPConn::IsStateOn()
{


	return bOn;
}

int CMyTCPConn::SetLastKeepalive(time_t tNow)
{

	return 0;
}

time_t CMyTCPConn::GetLastKeepalive()
{

	return tTime;
}

