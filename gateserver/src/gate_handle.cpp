#include <sys/time.h>

#include "../inc/gate_handle.h"
#include "../inc/gate_ctrl.h"
#include "../../framework/base/base.h"
#include "../../framework/message/message.pb.h"
#include "../../framework/message/tcpmessage.pb.h"
#include "../../framework/config/protoconfig.h"
#include "../../framework/net/client_comm_engine.h"


CGateHandle::CGateHandle()
{
	m_pInfo = NULL;
	m_eHandleType = EHandleType_FIRST;
	m_pConnsMap = NULL;
	m_tCheckStatLog = 0;

	memset((void *)&m_tvLastCheck, 0, sizeof(m_tvLastCheck));
}


CGateHandle::~CGateHandle()
{

}


int CGateHandle::Initialize(EMHandleType eHandleType, CDoubleLinkerInfo* pInfo, CONNS_MAP* pMap)
{
	m_eHandleType = eHandleType;
	m_pInfo = pInfo;
	m_pConnsMap = pMap;

	TFName szThreadLogFile;
	snprintf( szThreadLogFile, sizeof(szThreadLogFile)-1,"../log/handle%d.log", eHandleType);
#ifdef _DEBUG_
	ThreadLogInit(szThreadLogFile, 0x1000000, 5, 0, 700);
#else
	ThreadLogInit(szThreadLogFile, 0x1000000, 5, 0, 600);
#endif

	time(&m_tCheckStatLog);

	return 0;
}


int CGateHandle::IsToBeBlocked()
{
	int iRet = True;

	if(m_pInfo == NULL)
	{
		return iRet;
	}

#ifdef _POSIX_MT_
	pthread_mutex_lock(&CGateCtrl::stLinkMutex[m_eHandleType]);
#endif

	// 只检查当前线程所负责的conn
	CMyTCPConn* tpConn = (CMyTCPConn*) m_pInfo->GetHead();
	while (tpConn != NULL)
	{
		if (tpConn->IsConnCanRecv())
		{
			iRet =  False;
			break;
		}
		tpConn = (CMyTCPConn*) tpConn->GetNext();
	}

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&CGateCtrl::stLinkMutex[m_eHandleType]);
#endif

	return iRet;
}

int CGateHandle::MakeConnKey(const short nType, const short nID)
{
	int iKey = 0;

	iKey = nType;
	iKey = (iKey << 16) | nID;

	return iKey;
}


CMyTCPConn* CGateHandle::GetConnByKey(int iKey)
{
	CMyTCPConn* tpConn = NULL;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&CGateCtrl::stMutex[CGateCtrl::MUTEX_HASHMAP]);
#endif

	CONNS_MAP::iterator iter = m_pConnsMap->find(iKey);
	if (iter != m_pConnsMap->end())
	{
		tpConn = iter->second;
	}

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&CGateCtrl::stMutex[CGateCtrl::MUTEX_HASHMAP]);
#endif

	return tpConn;
}


int CGateHandle::SendOneCodeTo(short nCodeLength, BYTE* pbyCode, int  iKey, bool bKeepalive/* = false*/)
{
	CMyTCPConn *pWriteConn = NULL;
	int iTempRet = 0;
	
	if (nCodeLength <= 0 || !pbyCode)
	{
		TRACE_ERROR( "While send one code to (key=%d), null code.", iKey );
		return -10001;
	}

	pWriteConn = GetConnByKey(iKey);
	if (!pWriteConn)
	{
		TRACE_ERROR("While send one code to (key=%d), invalid key.", iKey);
		return -10002;
	}
	
	TRACE_DEBUG( "The real rcver is (%d, %d)", pWriteConn->GetEntityType(), pWriteConn->GetEntityID() );
	
	iTempRet = pWriteConn->SendCode(nCodeLength, pbyCode);
	if (iTempRet == 0 && bKeepalive == true)
	{
		// 设置keepalive时间
		pWriteConn->SetLastKeepalive(time(NULL));
	}
	
	return iTempRet;
}

int CGateHandle::TransferOneCode(short nCodeLength, BYTE* pbyCode)
{
	CTcpHead stTmpHead;

	int iTempRet = 0;
	int i;

	if (nCodeLength <= 0 || !pbyCode)
	{
		TRACE_ERROR("In TransferOneCode, invalid input params.");
		return -1;
	}

	unsigned short unOffset = 0;
	int iRet = ClientCommEngine::ConvertStreamToMsg(pbyCode, nCodeLength, unOffset, &stTmpHead);
	if (iRet < 0)
	{
		TRACE_ERROR("In TransferOneCode, ConvertStreamToCSHead return %d.", iRet);
		return -1;
	}

	TRACE_DEBUG( "TransMsg(%d).", nCodeLength);

#ifdef _DEBUG_
	TRACE_DEBUG("---- Recv Msg ----");
	TRACE_DEBUG("[%s]", stTmpHead.ShortDebugString().c_str());
#endif

	TRACE_DEBUG("Transfer code begin, from(FE = %d : ID = %d) to(FE = %d : ID = %d), timestamp = %ld.",
			stTmpHead.srcfe(), stTmpHead.srcid(),
			stTmpHead.dstfe(), stTmpHead.dstid(), stTmpHead.timestamp());

	// 处理直接发送到 gate 的消息
	if (stTmpHead.dstfe() == FE_GATESERVER)
	{
		switch(stTmpHead.opflag())
		{
			case EGC_KEEPALIVE:
			{
				CTcpHead stRetHead;
				stRetHead.Clear();

				pbmsg_settcphead(
						stRetHead,
						FE_GATESERVER,
						CConfigMgr::GetSingletonPtr()->GetGateConfig().id(),
						stTmpHead.srcfe(),
						stTmpHead.srcid(),
						time(NULL),
						EGC_KEEPALIVE);

				if (stRetHead.ByteSize() <= 0)
				{
					TRACE_DEBUG("stRetHead length = %d, invalid.", stRetHead.GetCachedSize());
					break;
				}

				// keepalive的包长度一般都很短
				char message_buffer[1024];
				unsigned short nLength = sizeof(message_buffer);
				int iRet = ClientCommEngine::ConvertMsgToStream(message_buffer, nLength, &stRetHead);
				if (iRet < 0)
				{
					TRACE_ERROR("send keepalive to (FE = %d : ID = %d), ConvertMsgToStream failed. iRet = %d.",
							stRetHead.dstfe(), stRetHead.dstid(), iRet);
				}
				else
				{
					int iKey = MakeConnKey(stRetHead.dstfe(), stRetHead.dstid());
					int iRet = SendOneCodeTo(nLength, (BYTE*)message_buffer, iKey, true);
					if (iRet != 0)
					{
						TRACE_ERROR("send keepalive to (FE = %d : ID = %d), SendOneCodeTo failed, iRet = %d.",
								stRetHead.dstfe(), stRetHead.dstid(), iRet);
					}
					else
					{
						TRACE_DEBUG("send keepalive to (FE = %d : ID = %d) succeed.", stRetHead.dstfe(), stRetHead.dstid());
					}
				}

				break;
			}
			default:
			{
				TRACE_ERROR("unknown command id %d, from(FE = %d : ID = %d) to(FE = %d : ID = %d), timestamp = %ld.",
						stTmpHead.opflag(), stTmpHead.srcfe(), stTmpHead.srcid(),
						stTmpHead.dstfe(), stTmpHead.dstid(), stTmpHead.timestamp());
				break;
			}
		}

		return 0;
	}

	int iKey = MakeConnKey(stTmpHead.dstfe(), stTmpHead.dstid());
	iTempRet = SendOneCodeTo(nCodeLength, pbyCode, iKey);
	m_stStatLog.iSndCnt++;
	m_stStatLog.iSndSize += nCodeLength;
	if (iTempRet)
	{
		TRACE_ERROR("transfer one code from (FE = %d : ID = %d) to (FE = %d : ID = %d) failed of %d.",
				stTmpHead.srcfe(), stTmpHead.srcid(), stTmpHead.dstfe(), stTmpHead.dstid(), iTempRet);
		m_stStatLog.iSndCnt--;
		m_stStatLog.iSndSize -= nCodeLength;
	}

	TRACE_DEBUG("Transfer code ended.");

	return 0;
}

int CGateHandle::DoTransfer()
{
	fd_set fds_read;
	struct timeval stMonTime;
	int i, iTempFD = -1;
	int iOpenFDNum = 0;
	int iTransferedCount = 0;
	int iTempRet = 0;
	int iIndex = 0;

	short nTmpCodeLength;
	// 性能优化分析
	BYTE  abyCodeBuf[MAX_PACKAGE_LEN];
	int iTempTimes;

	FD_ZERO( &fds_read );
	stMonTime.tv_sec = 0;
	stMonTime.tv_usec = 100000;

	std::vector<CMyTCPConn*> vecConns;

	#ifdef _POSIX_MT_
	pthread_mutex_lock(&CGateCtrl::stLinkMutex[m_eHandleType]);
#endif

	// 只检查当前线程所负责的conn
	for (CMyTCPConn* tpConn = (CMyTCPConn*) m_pInfo->GetHead(); tpConn != NULL; tpConn = (CMyTCPConn*) tpConn->GetNext())
	{
		if (tpConn->IsConnCanRecv())
		{
			tpConn->RegToCheckSet(&fds_read);
			vecConns.push_back(tpConn);
		}
	}

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&CGateCtrl::stLinkMutex[m_eHandleType]);
#endif

	// 等待接收
	iOpenFDNum = select(FD_SETSIZE, &fds_read, NULL, NULL, &stMonTime);

	if(iOpenFDNum <= 0)
	{
		return 0;
	}

	// 开始传输
	TRACE_DEBUG("--------Some entity has data to transfer----------.");

	// 只检查当前线程所负责的conn
	for (i = 0; i < vecConns.size(); i++ )
	{
		if (!vecConns[i])
		{
			continue;
		}

		// 不能接收则直接跳过
		if (!vecConns[i]->IsConnCanRecv())
		{
			continue;
		}
		
		if (vecConns[i]->IsFDSetted(&fds_read))
		{
			TRACE_DEBUG("conn(%d, %d) has some data to transfer.",
					vecConns[i]->GetEntityType(), vecConns[i]->GetEntityID());
			
			// 接收所能得到的所有数据
			vecConns[i]->RecvAllData();
			
			TRACE_DEBUG("Now transfer data...");

			while (1)
			{
				nTmpCodeLength = sizeof(abyCodeBuf);
				// 从接收到的数据队列中取数据
				iTempRet = vecConns[i]->GetOneCode(nTmpCodeLength, (BYTE *)abyCodeBuf);
				if (iTempRet > 0)
				{
					// 性能优化分析
					if (nTmpCodeLength < 6 || nTmpCodeLength > MAX_PACKAGE_LEN)
					{
						TRACE_ERROR("Get Error Len : %d Code from Conn (FE = %d , ID = %d)",
								nTmpCodeLength, vecConns[i]->GetEntityType(), vecConns[i]->GetEntityID());
						// 关闭掉连接
						vecConns[i]->GetSocket()->Close();
						vecConns[i]->SetConnState(ENTITY_OFF);
						m_stStatLog.iRcvErrCnt++;
						break;
					}

					// 转发数据
					if (TransferOneCode(nTmpCodeLength, (BYTE *)abyCodeBuf))
					{
						TRACE_ERROR("Conn(FE = %d, ID = %d) TransferOne code failed.",
								vecConns[i]->GetEntityType(), vecConns[i]->GetEntityID());
						// 关闭掉连接 
						vecConns[i]->GetSocket()->Close();
						vecConns[i]->SetConnState(ENTITY_OFF);
						break;
					}
					iTransferedCount++;
					m_stStatLog.iRcvCnt++;
					m_stStatLog.iRcvSize += nTmpCodeLength;
				}
				else
				{
					// 得不到数据
#ifdef _DEBUG_
					TRACE_DEBUG("DoTransfer : GetOneCode iTempRet = %d", iTempRet);
#endif
					if (iTempRet == -2)
					{
						TRACE_ERROR("len is impossblie! form Conn(FE = %d, ID = %d).",
								vecConns[i]->GetEntityType(), vecConns[i]->GetEntityID());
						vecConns[i]->GetSocket()->Close();
						vecConns[i]->SetConnState(ENTITY_OFF);
					}
                    break;
				}
			} //end while
		
			if (iTempRet == 0)
			{
				TRACE_DEBUG("All data transfered.");
			}
			else
			{
				TRACE_ERROR("Data error, recv buffer is resetted.");
			}
		} //end if
	} //end for
	
	TRACE_DEBUG("------ Thread %d do transfer OK -------.", m_eHandleType);

	// 返回一共转发了多少个数据
	return iTransferedCount;
}

int CGateHandle::CheckBlockCodes()
{
	timeval tvNow, tvGap;
	int iCleanCount = 0;
	int i;
	int iIndex = 0;

	gettimeofday(&tvNow, NULL);
	TimeValMinus(tvNow, m_tvLastCheck, tvGap);  // 计算出上次检查到现在的时间间隔
	if (tvGap.tv_sec < 1)
	{
		return 0;
	}

	m_tvLastCheck.tv_sec = tvNow.tv_sec;
	m_tvLastCheck.tv_usec = tvNow.tv_usec;

	int iReadBegin1 = 0;	
	int iReadEnd1 = 0;	
	int iWriteBegin1 = 0;	
	int iWriteEnd1 = 0;	
	int iReadBegin2 = 0;	
	int iReadEnd2 = 0;	
	int iWriteBegin2 = 0;	
	int iWriteEnd2 = 0;	

	int iBefore = 0;
	int iAfter = 0;

	std::vector<CMyTCPConn*> vecConns;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&CGateCtrl::stLinkMutex[m_eHandleType]);
#endif

	for (CMyTCPConn* tpConn = (CMyTCPConn*) m_pInfo->GetHead(); tpConn != NULL; tpConn = (CMyTCPConn*) tpConn->GetNext())
	{
		if (tpConn->IsConnCanRecv())
		{
			vecConns.push_back(tpConn);
		}
	}

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&CGateCtrl::stLinkMutex[m_eHandleType]);
#endif

	for (i = 0; i < vecConns.size(); i++)
	{
		iBefore = 0;
		iAfter = 0;
		if (vecConns[i]->IsConnCanRecv())
		{
			vecConns[i]->GetSocket()->GetCriticalData(iReadBegin1, iReadEnd1, iWriteBegin1, iWriteEnd1);
			iCleanCount += vecConns[i]->CleanBlockQueue(MAXCLEANBLOCKCODES);
			vecConns[i]->GetSocket()->GetCriticalData(iReadBegin2, iReadEnd2, iWriteBegin2, iWriteEnd2);
			iBefore = iWriteEnd1 - iWriteBegin1;
			iAfter = iWriteEnd2 - iWriteBegin2;
			if (iBefore > 0 && iAfter < iBefore)
			{
				m_stStatLog.iClnCnt++;
				m_stStatLog.iClnSize += (iBefore - iAfter);
				TRACE_DEBUG("Clean block codes len(%d) is cleaned.", (iBefore-iAfter));
			}
		}
	}

	if ( iCleanCount > 0 )
	{
		TRACE_DEBUG("---- Clean block codes ended, %d block codes is cleaned ----", iCleanCount);
	}

	return iCleanCount;
}

int CGateHandle::PrepareToRun()
{
	return 0;
}

int CGateHandle::Run()
{
	while( True )
	{
		// 一直休眠直到被唤醒
		CondBlock();

		// 真正的转发过程
		DoTransfer();
	
		// 发送完在DoTransfer过程中被Block的数据
		CheckBlockCodes();

		// 打印接收以及发送的信息，统计时长可能大于1分钟
		CheckStatLog();
	}
	
	return 0;
}

void CGateHandle::CheckStatLog()
{
	time_t tNow = time(NULL);
	if (tNow - m_tCheckStatLog >= 60)
	{
		m_tCheckStatLog = tNow;
		TRACE_INFO("-------------------------------- StatLog --------------------------------");
		TRACE_INFO("RcvCnt(%d), RcvSize(%d), RcvErrCnt(%d), SndCnt(%d), SndSize(%d), ClnCnt(%d), ClnSize(%d)",
				m_stStatLog.iRcvCnt, m_stStatLog.iRcvSize, m_stStatLog.iRcvErrCnt,
				m_stStatLog.iSndCnt, m_stStatLog.iSndSize, m_stStatLog.iClnCnt, m_stStatLog.iClnSize);
		m_stStatLog.Clear();
	}
}
