//
//  gate_conn.cpp
//  gate_conn 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include "../inc/gate_conn.h"

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

int CMyTCPConn::RegToCheckSet(fd_set *pCheckSet) {
	return iTempRet;
}

int CMyTCPConn::IsFDSetted(fd_set *pCheckSet) {
	int iTempRet = 0;

	return iTempRet;
}

int CMyTCPConn::RecvAllData() {


	return iTempRet;
}

int CMyTCPConn::GetOneCode(short &nCodeLength, BYTE *pCode) {
	return GetSocket()->GetOneCode((unsigned short int &)nCodeLength, pCode);
}

int CMyTCPConn::SendCode(short nCodeLength, BYTE *pCode, int iFlag /* = FLG_CONN_IGNORE */) {

	return iTempRet;
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

