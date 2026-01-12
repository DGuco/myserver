#include "wghomeplantopt.h"
#include "homeplant_manager.h"
#include "gcrethomeplant.h"
#include "cgaskhomeplant.h"

UINT WGHomePlantOptHandler::Execute(WGHomePlantOpt* pPacket, Player* pPlayer)
{
__ENTER_FUNCTION
    if(pPacket == NULL || pPlayer == NULL)
	{
		return PACKET_EXE_CONTINUE;
	}
	PlayerID_t PlayerID = pPacket->GetPlayerID();
	GamePlayer* pGamePlayer = g_pPlayerPool->GetPlayer(PlayerID);
	if( pGamePlayer==NULL )
	{
		return PACKET_EXE_CONTINUE;
	}

	Obj_Human* pHuman = pGamePlayer->GetHuman();
	Assert( pHuman );
	Scene* pScene = pHuman->getScene();
	if( pScene )
	{
        CacheGuid64Log(LOG_FILE_ERROR, "WGHomePlantOptHandler::Execute pScene == NULL (GUID=%s)", 
            GUID64String(pPacket->GetGUID()).GetString());    
		return PACKET_EXE_CONTINUE;
	}

	if( pPlayer->IsServerPlayer() )
	{//服务器收到世界服务器发来的数据
		AssertEx(g_pServerManager->VerifyExecuteThread(), "WGHomePlantOptHandler::Execute g_pServerManager->VerifyExecuteThread() == FALSE");
		pScene->PushAsyncPacket( pPacket, PlayerID );
		return PACKET_EXE_NOTREMOVE;
	}
	else if( pPlayer->IsGamePlayer() )
	{//场景收到Cache里的消息
		AssertEx(pScene->VerifyExecuteThread(), "WGHomePlantOptHandler::Execute pScene->VerifyExecuteThread() == FALSE");
        if(!pGamePlayer->IsCanLogic())
        {
            CacheGuid64Log(LOG_FILE_ERROR, "WGHomePlantOptHandler::Execute pGamePlayer->IsCanLogic() == FALSE (GUID=%s),name = %s", 
            GUID64String(pPacket->GetGUID()).GetString(), pHuman->GetName());    
	        return PACKET_EXE_CONTINUE;
        }
        
        if(pHuman->GetGUID() != pPacket->GetGUID())
        {
            CacheGuid64Log(LOG_FILE_ERROR, "WGHomePlantOptHandler::Execute pHuman->GetGUID() != pPacket->GetGUID() (GUID=%s),name = %s", 
            GUID64String(pPacket->GetGUID()).GetString(), pHuman->GetName());    
	        return PACKET_EXE_CONTINUE;
        }
	}
	else
	{
	    return PACKET_EXE_CONTINUE;
	}

    BYTE bOpt = pPacket->GetOpt();
    switch (bOpt)
    {
    case GWG_HOME_PLANT_OPT_RET_PLANT_INFO:
        {
            GCRetHomePlant pRetPlantInfo;
            pRetPlantInfo.SetHomePlant(*(pPacket->GetHomePlant()));
            pRetPlantInfo.SetOpt(HOME_PLANT_OPT_ASK);
            pHuman->SendPacket(pRetPlantInfo);
        }
        break;
    default:
        break;
    }
	return PACKET_EXE_CONTINUE;

    __LEAVE_FUNCTION
	return PACKET_EXE_ERROR;
}
