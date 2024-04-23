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
#include "server_client.h"
#include "message_factory.h"

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
// 	CSafePtr<CServerClient> pServerPlayer = pSocket.DynamicCastTo<CServerClient>();
// 	ASSERT(pServerPlayer != NULL && m_pMessage != NULL);
// 	const ProxyHead& mesHead = m_pMessage->msghead();
// 	int nFromT = mesHead.srcfe();
// 	int nFromId = mesHead.srcid();
// 	int nDestT = mesHead.dstfe();
// 	int nDestId = mesHead.dstid();
// 	int nCmdId = m_pMessage->msgcmd();
// 
// 	CSafePtr<CMessageFactory> pFactory = CMessageFactoryManager::GetSingletonPtr()->GetFactory(nCmdId);
// 	if (pFactory == NULL)
// 	{
// 		return 0;
// 	}
// 
// 	shared_ptr<ProtoMess> pServerMes = pFactory->CreateMessage();
// 	ASSERT(pServerMes != NULL);
// 	if (!pServerMes->ParseFromString(m_pMessage->buff()))
// 	{
// 		return 0;
// 	}
// 	pFactory->Execute(pSocket.DynamicCastTo<CTCPSocket>());
 	return 0;
}

#endif //__PROXY_MESSAGE_H__
