//
//  gate_handle.cpp
//  gate_handle 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <sys/time.h>
#include "../inc/gate_handle.h"
#include "../inc/gate_ctrl.h"
#include "../../framework/base/base.h"
#include "../../framework/message/message.pb.h"
#include "../../framework/message/tcpmessage.pb.h"
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

/**
  *函数名          : Initialize
  *功能描述        : 初始化线程
**/
int CGateHandle::Initialize(EMHandleType eHandleType, CDoubleLinkerInfo* pInfo, CONNS_MAP* pMap)
{
	m_eHandleType = eHandleType;
	m_pInfo = pInfo;
	m_pConnsMap = pMap;

	TFName szThreadLogFile;
	snprintf(szThreadLogFile,sizeof(szThreadLogFile) -1,"../log/handle%d.log",eHandleType);
#ifdef _DEBUG_
	ThreadLogInit(szThreadLogFile,0x1000000,5,0,700);
#else
	ThreadLogInit(szThreadLogFile,0x1000000,5,0,600);
#endif
	time(&m_tCheckStatLog);
	return 0;
}

/**
  *函数名          : IsToBeBlocked
  *功能描述        : 是否阻塞当前线程
**/
bool CGateHandle::IsToBeBlocked()
{
	bool iRet = true;
	if (m_pInfo == NULL)
	{
		return true;
	}

// 该过程需要在线程锁内完成
#ifdef _POSIX_MT_
	std::lock_guard<std::mutex> guard(CGateCtrl::stLinkMutex[m_eHandleType]);
#endif

	//检查当前线程的所有连接是否可读
	CMyTCPConn* tcpConn = (CMyTCPConn*) m_pInfo->GetHead();
	while(tcpConn != NULL)
	{
        //如果有一个连接可读，则唤醒改线程
		if (tcpConn->IsConnCanRecv())
		{
			iRet = false;
			break;
		}
		tcpConn = (CMyTCPConn*) tcpConn->GetNext();
	}

	return iRet;
}

/**
  *函数名          : MakeConnKey
  *功能描述        : 生成key
**/
int CGateHandle::MakeConnKey(const short nType, const short nID)
{
	int iKey = 0;

	iKey = nType;
	iKey = (iKey << 16) | nID;

	return iKey;
}

/**
  *函数名          : GetConnByKey
  *功能描述        : 获取i一个连接
**/
CMyTCPConn* CGateHandle::GetConnByKey(int iKey)
{
    CMyTCPConn* tpConn = NULL;
#ifdef _POSIX_MT_
    std::lock_guard<std::mutex> guard(CGateCtrl::stMutex[CGateCtrl::MUTEX_HASHMAP]);
#endif

    CONNS_MAP::iterator iter = m_pConnsMap->find(iKey);
    if (iter != m_pConnsMap->end())
    {
        tpConn = iter->second;
    }
    return tpConn;
}

/**
  *函数名          : SendOneCodeTo
  *功能描述        : 发送一个数据包
**/
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

/**
  *函数名          : TransferOneCode
  *功能描述        : 转发一个数据包
**/
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
            //心跳信息
            case EGC_KEEPALIVE:
            {
                CTcpHead stRetHead;
                stRetHead.Clear();
                pbmsg_settcphead(
                        stRetHead,
                        FE_GATESERVER,
                        CServerConfig::GetSingletonPtr()->m_iGateServerId,
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
    int i,iTmpFd = -1;
    int iOpenFdNum = 0;
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
    std::unique_lock<std::mutex> lk(CGateCtrl::stLinkMutex[m_eHandleType]);
    lk.lock();
#endif
    //检查当前线程的连接
    for (CMyTCPConn* tpConn = (CMyTCPConn*)m_pInfo->GetHead();tpConn != NULL,tpConn = (CMyTCPConn*)tpConn->GetNext())
    {
        if (tpConn->IsConnCanRecv())
        {
            //把socket添加到可读集合中
            tpConn->RegToCheckSet(&fds_read);
            vecConns.push_back(tpConn);
        }
    }
#ifdef _POSIX_MT_
    lk.unlock();
#endif

	return 0;
}

int CGateHandle::CheckBlockCodes()
{
	int iCleanCount = 0;

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
}
