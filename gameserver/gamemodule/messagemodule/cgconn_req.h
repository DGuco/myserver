/*****************************************************************
* FileName:cgconn_req.h
* Summary :
* Date	  :2024-3-25
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __CGCONN_REQ_H__
#define __CGCONN_REQ_H__

#include "message.pb.h"
#include "game_player.h"
#include "message_factory.h"

class CGConnReq : public CGameMess, public CGonnProto
{
public:
	CGConnReq()  {};
	virtual ~CGConnReq() {};
	virtual int Execute(CSafePtr<CTCPSocket> pSocket);
};


class CCGConnReqFacory : public CMessageFactory
{
public:
	CCGConnReqFacory() {};
	virtual ~CCGConnReqFacory() {};
	virtual CSafePtr<CGameMess> CreateMessage()
	{
		CSafePtr<CGConnReq> pMess = new CGConnReq();
		return pMess.DynamicCastTo<CGameMess>();
	}
};

int CGConnReq::Execute(CSafePtr<CTCPSocket> pSocket);
{
	CSafePtr<CGamePlayer> pGamePlayer = pSocket.DynamicCastTo<CGamePlayer>();
	ASSERT(pGamePlayer);
	std::string pAccount = account();
	std::string pPssward = password();
	int pFrom = pFrom();
	std::string pSession = session();
	return 0;
}

#endif //__CGCONN_REQ_H__
