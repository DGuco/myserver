/*****************************************************************
* FileName:proxy_message.h
* Summary :
* Date	  :2024-3-26
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __PROXY_MESSAGE_H__
#define __PROXY_MESSAGE_H__

#include "safe_pointer.h"
#include "message.pb.h"
#include "proxy_player.h"
#include "message_factory.h"
#include "time_helper.h"
#include "proxy_server.h"
#include "log.h"

class CProxyMessageFactory : public CMessageFactory
{
public:
	CProxyMessageFactory() {};
	virtual ~CProxyMessageFactory() {};
	virtual shared_ptr<ProtoMess>	CreateMessage();
	virtual int						MessId();
	virtual int						Execute(CSafePtr<CTCPSocket> pSocket);
public:
	shared_ptr<ProxyMessage>		m_pMessage;
};

shared_ptr<ProtoMess> CProxyMessageFactory::CreateMessage()
{
	m_pMessage = std::make_shared<ProxyMessage>();
	return std::dynamic_pointer_cast<ProtoMess>(m_pMessage);
}

int	CProxyMessageFactory::MessId()
{
	return ProxyMessage::Msg::ProxyMessage_Msg_MsgID;
}

int CProxyMessageFactory::Execute(CSafePtr<CTCPSocket> pSocket)
{
	CSafePtr<CProxyPlayer> pProxyPlayer = pSocket.DynamicCastTo<CProxyPlayer>();
	ASSERT(pProxyPlayer != NULL && m_pMessage != NULL);
	int nFromT = m_pMessage->srcfe();
	int nFromId = m_pMessage->srcid();
	int nDestT = m_pMessage->dstfe();
	int nDestId = m_pMessage->dstid();
	int nCmdId = m_pMessage->cmd();
	time_t sendtime = m_pMessage->timestamp();
	int nPacketId = m_pMessage->packetid();

	if (pProxyPlayer->GetProxyState() == eProConnected)
	{
		if (nCmdId != enMessageCmd::MESS_REGISTER)
		{
			//直接踢掉
			pProxyPlayer->SetProxyState(eProKicking);
			return 0;
		}
		pProxyPlayer->SetServerType((enServerType)nFromT);
		pProxyPlayer->SetServerId((enServerType)nFromId);
		pProxyPlayer->SetProxyState(eProRegistered);
		//注册连接
		CProxyServer::GetSingletonPtr()->RegisterNewConn(pProxyPlayer);
		return 0;
	}

	if (pProxyPlayer->GetProxyState() != eProRegistered)
	{
		//直接踢掉
		pProxyPlayer->SetProxyState(eProKicking);
		return 0;
	}

	//心跳
	if (nCmdId == enMessageCmd::MESS_KEEPALIVE)
	{
		pProxyPlayer->SetLastRecvKeepAlive(CTimeHelper::GetSingletonPtr()->GetANSITime());
		CACHE_LOG(TCP_DEBUG, "Recv keepalive from {}:{} to {}:{}", nFromT, nFromId, nDestT, nDestId);
		return 0;
	}

	CProxyServer::GetSingletonPtr()->TransferMessage(pProxyPlayer,nDestT, nDestId, m_pMessage);
	return 0;
}

#endif //__PROXY_MESSAGE_H__
