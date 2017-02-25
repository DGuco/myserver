#include "../inc/gate_conn.h"

int CMyTCPConn::EstConn(int iAcceptFD) {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	iTempRet = GetSocket()->Accept(iAcceptFD);

	m_iConnState = ENTITY_OFF;

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::IsConnCanRecv() {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif
	if (GetSocket()->GetSocketFD() > 0 && GetSocket()->GetStatus() == tcs_connected) {
		iTempRet = True;
	} else {
		iTempRet = False;
	}
#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::RegToCheckSet(fd_set *pCheckSet) {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	iTempRet = GetSocket()->AddToCheckSet(pCheckSet);

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::IsFDSetted(fd_set *pCheckSet) {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif
	// 查看该套接字的FD是否在套接字集合中
	iTempRet = GetSocket()->IsFDSetted(pCheckSet);

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::RecvAllData() {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	iTempRet = GetSocket()->RecvData();

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::GetOneCode(short &nCodeLength, BYTE *pCode) {
	return GetSocket()->GetOneCode((unsigned short int &)nCodeLength, pCode);
}

int CMyTCPConn::SendCode(short nCodeLength, BYTE *pCode, int iFlag /* = FLG_CONN_IGNORE */) {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	if (GetEntityType() < 0 || GetEntityID() < 0) {
		// 未配置节点直接返回 
#ifdef _POSIX_MT_
		pthread_mutex_unlock(&m_stMutex);
#endif
		return -101;
	}

	if (iFlag == FLG_CONN_CTRL || iFlag == FLG_CONN_IGNORE) {
		// 外带数据和只读操作优先发送, 不参与排队
		if (GetSocket()->GetSocketFD() > 0 && GetSocket()->GetStatus() == tcs_connected) {
			iTempRet = GetSocket()->SendOneCode(nCodeLength, pCode);
		} else {
			iTempRet = -102;
		}

#ifdef _POSIX_MT_
		pthread_mutex_unlock(&m_stMutex);
#endif
		return iTempRet;
	}

	return iTempRet;
}

int CMyTCPConn::CleanBlockQueue(int iQueueLength) {
	int iRedoCount = 0, i, iTempRet = 0;
	int nCodeLength;
	BYTE  abyCodeBuf[MAX_PACKAGE_LEN];

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	if (m_iConnState == ENTITY_ON && GetSocket()->GetSocketFD() > 0 &&
			GetSocket()->GetStatus() == tcs_connected) {
		// 如果有滞留数据，则重新发送，知道发送完成或出错
		if (GetSocket()->HasReserveData() == True) {
			LOG_DEBUG("default", "CleanReserveData begin.");
			iTempRet = GetSocket()->CleanReserveData();
			if (iTempRet != 0)
			{
#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif
				LOG_ERROR("default", "CleanReserveData failed. iTempRet = %d.", iTempRet);
				return 0;
			}
			else
			{
#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif
				return 1;
			}
		}
	}
#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	//return iRedoCount;
	return 0;
}

int CMyTCPConn::SetConnState(int iConnState) {
#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	m_iConnState = iConnState;

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return 0;
}

bool CMyTCPConn::IsStateOn()
{
	bool bOn = false;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	bOn = (m_iConnState == ENTITY_ON);

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return bOn;
}

int CMyTCPConn::SetLastKeepalive(time_t tNow)
{
#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	m_tLastKeepalive = tNow;

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return 0;
}

time_t CMyTCPConn::GetLastKeepalive()
{
	time_t tTime = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	tTime = m_tLastKeepalive;

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return tTime;
}

