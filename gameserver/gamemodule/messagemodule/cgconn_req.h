/*****************************************************************
* FileName:cgconn_req.h
* Summary :
* Date	  :2024-3-25
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __CGCONN_REQ_H__
#define __CGCONN_REQ_H__

#include "safe_pointer.h"
#include "message.pb.h"
#include "game_player.h"
#include "message_factory.h"

class CCGConnReqFacory : public CMessageFactory
{
public:
	CCGConnReqFacory() {};
	virtual ~CCGConnReqFacory() {};
	virtual shared_ptr<ProtoMess> CreateMessage();
	virtual int					MessId();
	virtual int					Execute(CSafePtr<CTCPSocket> pSocket);
public:
	shared_ptr<CGonnReq>		m_pMessage;
};

shared_ptr<ProtoMess> CCGConnReqFacory::CreateMessage()
{
	m_pMessage = std::make_shared<CGonnReq>();
	return std::dynamic_pointer_cast<ProtoMess>(m_pMessage);
}

int	CCGConnReqFacory::MessId()
{
	return CGonnReq::Msg::CGonnReq_Msg_MsgID;
}

int CCGConnReqFacory::Execute(CSafePtr<CTCPSocket> pSocket)
{
	CSafePtr<CGamePlayer> pGamePlayer = pSocket.DynamicCastTo<CGamePlayer>();
	ASSERT(pGamePlayer != NULL && m_pMessage != NULL);
	std::string pAccount = m_pMessage->account();
	std::string pPssward = m_pMessage->password();
	int pFrom = m_pMessage->pfrom();
	std::string pSession = m_pMessage->session();
	return 0;
}

#endif //__CGCONN_REQ_H__
