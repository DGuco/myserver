#include "cgaskhomeplant.h"
#include "gwhomeplantopt.h"

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
	BYTE opt = pPacket->GetOpt();
	INT bGWOpt = -1;
	switch(opt)
	{
		case HOME_PLANT_OPT_ASK:
			{
				bGWOpt = GWG_HOME_PLANT_OPT_ASK;
			}
			break;
		case HOME_PLANT_OPT_UNLOCK_TRUNK:
			{
				bGWOpt = GWG_HOME_PLANT_OPT_UNLOCK_TRUNK;
			}
			break;
		case HOME_PLANT_OPT_WATER:
			{
				bGWOpt = GWG_HOME_PLANT_OPT_WATER;
			}
			break;
		case HOME_PLANT_OPT_FERTILIZE:
			{
				bGWOpt = GWG_HOME_PLANT_OPT_FERTILIZE;
			}
			break;
		default:
			break;
	}

	if(bGWOpt == -1)
	{
		return PACKET_EXE_ERROR;
	}
	GWHomePlantOpt* pHomePlantOpt = (GWHomePlantOpt*)g_pPacketFactoryManager->CreatePacket(PACKET_GW_HOME_PLANT_OPT);
	ASSERT(pHomePlantOpt);
	pHomePlantOpt->SetGUID(pHuman->GetGUID());
	pHomePlantOpt->SetOpt(bGWOpt);
	g_pServerManager->PushAsyncPacket(pHomePlantOpt,pHuman->GetZoneWorldID());
    return PACKET_EXE_CONTINUE;
    __LEAVE_FUNCTION
    return 0;
}

