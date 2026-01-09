#include "cgaskhomeplant.h"
#include  "gwghomeplantopt.h"

UINT CGAskHomePlantHandler::Execute(CGAskHomePlant* pPacket,Player* pPlayer)
{
    __ENTER_FUNCTION
	ASSERT( pPacket );
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
    AssertEx(true == pScene->VerifyExecuteThread(),"CGAskHomePlantHandler::Execute pScene->VerifyExecuteThread() failed");
	GWGHomePlantOpt* pHomePlantOpt = (GWGHomePlantOpt*)g_pPacketFactoryManager->CreatePacket(PACKET_GW_HOME_PLANT_OPT);
	ASSERT(pHomePlantOpt);
	g_pServerManager->PushAsyncPacket(pHomePlantOpt,pHuman->GetZoneWorldID());
    return PACKET_EXE_CONTINUE;
    __LEAVE_FUNCTION
    return 0;
}

