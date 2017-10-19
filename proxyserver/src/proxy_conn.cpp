//
//  gate_conn.cpp
//  gate_conn 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include "../inc/proxy_conn.h"

/**
  *函数名          : EstConn
  *功能描述        : 建立连接
**/
int CMyTCPConn::EstConn(int iAcceptFD)
{
    int iTempRet = 0;
    iTempRet = GetSocket()->Accept(iAcceptFD);
    {
        #ifdef _POSIX_MT_
                std::lock_guard<std::mutex> guard(m_stMutex);
        #endif
        m_iConnState = ENTITY_OFF;
    }
	return iTempRet;
}

/**
  *函数名          : IsConnCanRecv
  *功能描述        : 检测连接是否准备好接受数据
**/
bool CMyTCPConn::IsConnCanRecv()
{
    bool iTmpRet;
    if (GetSocket()->GetSocketFD() > 0 && GetSocket()->GetStatus() == tcs_connected)
    {
        iTmpRet  = True;
    }
    else
    {
        iTmpRet =  False;
    }
    return iTmpRet;
}
/**
  *函数名          : RegToCheckSet
  *功能描述        : 把socket添加到pCheckSet集合中
**/
int CMyTCPConn::RegToCheckSet(fd_set *pCheckSet) {
    int iTmpRet = 0;
    iTmpRet = GetSocket()->AddToCheckSet(pCheckSet);
	return iTmpRet;
}

/**
  *函数名          : IsFDSetted
  *功能描述        : 检测socket是否添加到pCheckSet集合中
**/
int CMyTCPConn::IsFDSetted(fd_set *pCheckSet) {
	int iTmpRet = 0;
	// 查看该套接字的FD是否在套接字集合中
	iTmpRet = GetSocket()->IsFDSetted(pCheckSet);
	return iTmpRet;
}

/**
  *函数名          : RecvAllData
  *功能描述        : 接受所有数据
**/
int CMyTCPConn::RecvAllData()
{
	int iTmpRet = 0;
    iTmpRet =  GetSocket()->RecvData();
	return iTmpRet;
}

/**
  *函数名          : GetOneCode
  *功能描述        : 接受指定长度的数据
**/
int CMyTCPConn::GetOneCode(short &nCodeLength, BYTE *pCode)
{
	return GetSocket()->GetOneCode((unsigned short int &)nCodeLength, pCode);
}

/**
  *函数名          : SendCode
  *功能描述        : 发送指定长度的数据
**/
int CMyTCPConn::SendCode(short nCodeLength, BYTE *pCode, int iFlag /* = FLG_CONN_IGNORE */)
{
	int iTmpRet = 0;
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

/**
  *函数名          : CleanBlockQueue
  *功能描述        : 发送滞留数据
**/
int CMyTCPConn::CleanBlockQueue(int iQueueLength)
{
    int iRedoCount,i,iTmpRet = 0;
    int nCodeLength = 0;
    BYTE abcCodeBuf[MAX_PACKAGE_LEN];

#ifdef _POSIX_MT_
    std::lock_guard<std::mutex> guard(m_stMutex);
#endif

    if (m_iConnState == ENTITY_ON && GetSocket()->GetSocketFD() > 0
            && GetSocket()->GetStatus() == tcs_connected)
    {
        // 如果有滞留数据，则重新发送，直到发送完成或出错
        if (GetSocket()->HasReserveData() == True)
        {
            iTmpRet = GetSocket()->CleanReserveData();
            if (iTmpRet != 0)
            {
                LOG_ERROR("default", "CleanReserveData failed. iTempRet = %d.", iTmpRet);
                return 0;
            }
        }
    }
	return 0;
}

/**
  *函数名          : SetConnState
  *功能描述        : 设置连接状态
**/
int CMyTCPConn::SetConnState(int iConnState)
{
    {
        #ifdef _POSIX_MT_
            std::lock_guard<std::mutex> guard(m_stMutex);
        #endif
        m_iConnState = iConnState;
    }
	return 0;
}

/**
  *函数名          : IsStateOn
  *功能描述        : 连接是否建立
**/
bool CMyTCPConn::IsStateOn()
{
    bool isOn;
    {
        #ifdef _POSIX_MT_
            std::lock_guard<std::mutex> guard(m_stMutex);
        #endif
        isOn = (m_iConnState == ENTITY_ON);
    }

	return isOn;
}

/**
  *函数名          : SetLastKeepalive
**/
int CMyTCPConn::SetLastKeepalive(time_t tNow)
{
    {
        #ifdef _POSIX_MT_
            std::lock_guard<std::mutex> guard(m_stMutex);
        #endif
        m_tLastKeepalive = tNow;
    }

	return 0;
}

/**
  *函数名          : GetLastKeepalive
**/
time_t CMyTCPConn::GetLastKeepalive()
{
    time_t  tTime = 0;
    {
        #ifdef _POSIX_MT_
            std::lock_guard<std::mutex> guard(m_stMutex);
        #endif
        tTime = m_tLastKeepalive;
    }
	return tTime;
}

