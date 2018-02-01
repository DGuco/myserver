//
// Created by dguco on 18-1-30.
//

#include <sharemem.h>
#include <my_assert.h>
#include <acceptor.h>
#include <client_comm_engine.h>
#include "net_work.h"
#include "../inc/s2c_handle.h"
#include "../inc/gate_def.h"
#include "../inc/gate_ctrl.h"

CCodeQueue *CS2cHandle::m_pS2CPipe = NULL;

CS2cHandle::CS2cHandle()
	: m_iSendIndex(0),
	  m_bHasRecv(false),
	  m_iSCIndex(0),
	  m_nSCLength(0)
{
}

CS2cHandle::~CS2cHandle()
{

}

int CS2cHandle::PrepareToRun()
{
	return 0;
}

int CS2cHandle::Run()
{
	while (true) {
		//如果有数据需要发送
		if (!IsToBeBlocked()) {
			CheckWaitSendData();
		}
		else {
			usleep(500);
		}
	}
}

bool CS2cHandle::IsToBeBlocked()
{
	return m_pS2CPipe->IsQueueEmpty();
}

void CS2cHandle::CheckWaitSendData()
{
	int iTmpRet = 0;
	int unTmpCodeLength = 0;

	if (m_iSendIndex < m_oMesHead.socketinfos().size()) {
		//有数据未发送，继续发送
		SendClientData();
	}
	else {
		//当前没有接收到数据，先接收数据
		if (false == m_bHasRecv) {
			//没有可发送的数据或者发送完成,则接收gate数据
			iTmpRet = RecvServerData();
			//没有数据可接收，则发送队列无数据发送，退出
			if (iTmpRet == 0) {
			}
			m_bHasRecv = true;
		}//处理已经接收到的数据
		else {
			//组织服务器发送到客户端的数据信息头，设置相关索引和游标
			m_iSendIndex = 0;
			m_iSCIndex = 0;
			m_nSCLength = 0;
			//反序列化消息的CTcpHead,取出发送游标和长度,把数据存入发送消息缓冲区m_szMsgBuf
			iTmpRet = CClientCommEngine::ConvertStreamToMessage(m_acSCMsgBuf,
																unTmpCodeLength,
																&m_oMesHead,
																NULL,
																NULL,
																&m_iSendIndex);
			//序列化失败继续发送
			if (iTmpRet < 0) {
				LOG_ERROR("default",
						  "CTCPCtrl::CheckWaitSendData Error, ClientCommEngine::ConvertMsgToStream return %d.",
						  iTmpRet);
				m_oMesHead.Clear();
				m_iSendIndex = 0;
				return;
			}

			//接收成功,取出数据长度
			char *pTmp = m_acSCMsgBuf;
			pTmp += m_iSCIndex;
			m_nSCLength = *(unsigned short *) pTmp;
		}
	}
}

int CS2cHandle::SendClientData()
{
	BYTE *pbTmpSend = NULL;
	unsigned short unTmpShort;
	int nTmpIndex;
	unsigned short unTmpPackLen;

	auto *pSendList = m_oMesHead.mutable_socketinfos();
	//client socket索引非法，不存在要发送的client
	if (m_iSendIndex >= pSendList->size())
		return 0;

	unTmpPackLen = m_nSCLength;
	//发送数据
	if (unTmpPackLen > 0) {
		//根据发送给客户端的数据在m_szSCMsgBuf中的数组下标取出数据
		pbTmpSend = (BYTE *) m_acSCMsgBuf[m_iSCIndex];
		memcpy((void *) &unTmpShort, (const void *) pbTmpSend, sizeof(unsigned short));
		if (unTmpShort != unTmpPackLen) {
			LOG_ERROR("default",
					  "Code length not matched,left length %u is less than body length %u",
					  unTmpPackLen,
					  unTmpShort);
			return -1;
		}
	}

	for (int i = 0; i < pSendList->size(); ++i) {
		//向后移动socket索引
		m_iSendIndex++;
		CSocketInfo tmpSocketInfo = pSendList->Get(m_iSendIndex);
		nTmpIndex = tmpSocketInfo.socketid();
		//socket 非法
		if (nTmpIndex <= 0 || MAX_SOCKET_NUM <= nTmpIndex) {
			LOG_ERROR("default", "Invalid socket index %d", nTmpIndex);
			continue;
		}
		CGateCtrl::GetSingletonPtr()->GetSingThreadPool()->PushTaskBack(
			[&tmpSocketInfo, pbTmpSend, unTmpPackLen, this]
			{
				SendToClient(tmpSocketInfo, (const char *) pbTmpSend, unTmpPackLen);
			}
		);
	}
	return 0;
}

int CS2cHandle::RecvServerData()
{
	int unTmpCodeLength = MAX_PACKAGE_LEN;
	if (m_pS2CPipe->GetHeadCode((BYTE *) m_acSCMsgBuf, &unTmpCodeLength) < 0) {
		unTmpCodeLength = 0;
	}
	return unTmpCodeLength;
}

void CS2cHandle::SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len)
{
// 该过程需要在线程锁内完成
	CAcceptor *pTmpAcceptor = CNetWork::GetSingletonPtr()->FindAcceptor(socketInfo.socketid());
	if (pTmpAcceptor == NULL) {
		LOG_ERROR("default", "CAcceptor has gone socket %d", socket);
		return;
	}
	/*
	 * 时间不一样，说明这个socket是个新的连接，原来的连接已经关闭,注(原来的
	 * 的连接断开后，新的客户端用了原来的socket fd ，因此数据不是现在这个连
	 * 接的数据，原来连接的数据,中断发送
	*/
	if (pTmpAcceptor->GetCreateTime() != socketInfo.createtime()) {
		LOG_ERROR("default",
				  "sokcet[%d] already closed(tcp createtime:%d:gate createtime:%d) : gate ==> client[%d] bytes failed",
				  socket,
				  pTmpAcceptor->GetCreateTime(),
				  socketInfo.createtime(),
				  m_nSCLength);
		return;
	}
	int iTmpCloseFlag = socketInfo.state();
	int iRet = pTmpAcceptor->Send(data, len);
	if (iRet != 0) {
		//发送失败
		CC2sHandle::ClearSocket(pTmpAcceptor, Err_ClientClose);
		LOG_ERROR("default",
				  "send to client %s Failed due to error %d",
				  pTmpAcceptor->GetSocket().GetSocket(),
				  errno);
		return;
	}

	//gameserver 主动关闭
	if (iTmpCloseFlag < 0) {
		CC2sHandle::ClearSocket(pTmpAcceptor, Client_Succeed);
	}
}
