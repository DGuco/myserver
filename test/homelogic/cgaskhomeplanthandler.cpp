#include "cgaskhomeplant.h"

UINT CGAskHomePlantHandler::Execute(CGAskHomePlant* pPacket,Player* pPlayer)
{
    __ENTER_FUNCTION
    GamePlayer* pGamePlayer = (GamePlayer*)pPlayer ;
	Assert( pGamePlayer ) ;

	Obj_Human* pHuman = pGamePlayer->GetHuman() ;
	Assert( pHuman ) ;
	
	Scene* pScene = pHuman->getScene() ;
	if( pScene==NULL )
	{
		Assert(FALSE) ;
		return PACKET_EXE_ERROR ;
	}

    Assert(true == pScene->VerifyExecuteThread());
    return PACKET_EXE_CONTINUE;
    __LEAVE_FUNCTION
    return 0;
}

