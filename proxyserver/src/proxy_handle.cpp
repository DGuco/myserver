//
//  gate_handle.cpp
//  gate_handle 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <sys/time.h>
#include "../inc/proxy_handle.h"
#include "../inc/proxy_ctrl.h"
#include "../../framework/message/message.pb.h"
#include "../../framework/net/server_comm_engine.h"


CProxyHandle::CProxyHandle()
{
	m_pInfo = NULL;
	m_eHandleType = EHandleType_FIRST;
	m_pConnsMap = NULL;
	m_tCheckStatLog = 0;

	memset((void *)&m_tvLastCheck, 0, sizeof(m_tvLastCheck));
}


CProxyHandle::~CProxyHandle()
{

}

/**
  *函数名          : Initialize
  *功能描述        : 初始化线程
**/
int CProxyHandle::Initialize(EMHandleType eHandleType, CDoubleLinkerInfo* pInfo, CONNS_MAP* pMap)
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
bool CProxyHandle::IsToBeBlocked()
{
	bool iRet = true;
	if (m_pInfo == NULL)
	{
		return true;
	}

    {
        // 该过程需要在线程锁内完成
        #ifdef _POSIX_MT_
            std::lock_guard<std::mutex> guard(CProxyCtrl::stLinkMutex[m_eHandleType]);
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
    }

	return iRet;
}

/**
  *函数名          : MakeConnKey
  *功能描述        : 生成key
**/
int CProxyHandle::MakeConnKey(const short nType, const short nID)
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
CMyTCPConn* CProxyHandle::GetConnByKey(int iKey)
{
    CMyTCPConn* tpConn = NULL;
#ifdef _POSIX_MT_
    std::lock_guard<std::mutex> guard(CProxyCtrl::stMutex[CProxyCtrl::MUTEX_HASHMAP]);
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
int CProxyHandle::SendOneCodeTo(short nCodeLength, BYTE* pbyCode, int  iKey, bool bKeepalive/* = false*/)
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
int CProxyHandle::TransferOneCode(short nCodeLength, BYTE* pbyCode)
{
    CProxyMessage stTmpMessage;

    int iTempRet = 0;
    if (nCodeLength <= 0 || !pbyCode)
    {
        TRACE_ERROR("In TransferOneCode, invalid input params.");
        return -1;
    }

    unsigned short unOffset = 0;
    int iRet = ServerCommEngine::ConvertStreamToMsg(pbyCode, nCodeLength,&stTmpMessage);
    if (iRet < 0)
    {
        TRACE_ERROR("In TransferOneCode, ConvertStreamToCSHead return %d.", iRet);
        return -1;
    }

    CProxyHead stTmpHead = stTmpMessage.msghead();
    TRACE_DEBUG( "TransMsg(%d).", nCodeLength);

#ifdef _DEBUG_
    TRACE_DEBUG("---- Recv Msg ----");
    TRACE_DEBUG("[%s]", stTmpHead.ShortDebugString().c_str());
#endif

    TRACE_DEBUG("Transfer code begin, from(FE = %d : ID = %d) to(FE = %d : ID = %d), timestamp = %ld.",
                stTmpHead.srcfe(), stTmpHead.srcid(),
                stTmpHead.dstfe(), stTmpHead.dstid(), stTmpHead.timestamp());

    // 处理直接发送到 proxy 的消息
    if (stTmpHead.dstfe() == FE_PROXYSERVER)
    {
        switch(stTmpHead.opflag())
        {
            case enMessageCmd::MESS_KEEPALIVE:
            {
                CProxyHead stRetHead;
                ServerInfo* serverInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
                stRetHead.set_srcfe(FE_PROXYSERVER);
                stRetHead.set_srcid(serverInfo->m_iServerId);
                stRetHead.set_dstfe(stTmpHead.srcfe());
                stRetHead.set_dstid(stTmpHead.srcid());
                stRetHead.set_opflag(enMessageCmd::MESS_KEEPALIVE);
                stRetHead.set_timestamp(GetMSTime());

                // keepalive的包长度一般都很短
                char message_buffer[1024];

                MSG_LEN_TYPE unHeadLen = stRetHead.ByteSize();
                MSG_LEN_TYPE unAddLen = ((6 + unHeadLen) % 8);
                if (unAddLen > 0)
                {
                    unAddLen = (8 - unAddLen);
                }
                MSG_LEN_TYPE unTotalLen = (unHeadLen + 6 + unAddLen);

                int typeLen = sizeof(MSG_LEN_TYPE);
                *((MSG_LEN_TYPE*) message_buffer) = unTotalLen;
                *((MSG_LEN_TYPE*) (message_buffer + typeLen * 1)) = unAddLen;
                *((MSG_LEN_TYPE*) (message_buffer + typeLen * 2)) = unHeadLen;

                if (stRetHead.SerializeToArray((message_buffer + 6), sizeof(message_buffer) - 6) == false)
                {
                    TRACE_ERROR("send keepalive to (FE = %d : ID = %d), CProxyHead::SerializeToArray failed.",
                                stRetHead.dstfe(), stRetHead.dstid());
                }
                else
                {
                    int iKey = MakeConnKey(stRetHead.dstfe(), stRetHead.dstid());
                    int iRet = SendOneCodeTo(unTotalLen, (BYTE*)message_buffer, iKey, true);
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

int CProxyHandle::DoTransfer()
{
    fd_set fds_read;
    struct timeval stMonTime;
    int iOpenFDNum = 0;
    int iTransferedCount = 0;
    int iTempRet = 0;
    int i;
    short nTmpCodeLength = 0;

    // 性能优化分析
    BYTE  abyCodeBuf[MAX_PACKAGE_LEN];
    FD_ZERO( &fds_read );
    stMonTime.tv_sec = 0;
    stMonTime.tv_usec = 100000;
    std::vector<CMyTCPConn*> vecConns;
#ifdef _POSIX_MT_
    std::unique_lock<std::mutex> lk(CProxyCtrl::stLinkMutex[m_eHandleType]);
#endif
    //检查当前线程的连接
    for (CMyTCPConn* tpConn = (CMyTCPConn*)m_pInfo->GetHead();tpConn != NULL;tpConn = (CMyTCPConn*)tpConn->GetNext())
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
        }
    }

    TRACE_DEBUG("------ Thread %d do transfer OK -------.", m_eHandleType);
    // 返回一共转发了多少个数据
    return iTransferedCount;
}

int CProxyHandle::CheckBlockCodes()
{
	int iCleanCount = 0;

	return iCleanCount;
}

int CProxyHandle::PrepareToRun()
{
	return 0;
}

int CProxyHandle::Run()
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

void CProxyHandle::CheckStatLog()
{
}
