#include "gwhomeplantopt.h"
#include "wghomeplantopt.h"
#include "homeplant_manager.h"

UINT GWHomePlantOptHandler::Execute(GWHomePlantOpt* pPacket, Player* pPlayer)
{
    __ENTER_FUNCTION
	ASSERT( pPacket );
    USER* pUser = g_OnlineUser->FindUser(pPacket->GetGUID());
    if(pUser == NULL)
    {
        CacheGuid64Log(LOG_FILE_WORLD, "GWGHomePlantOptHandler::Execute, user not found, guid = %s",GUID64String(pPacket->GetGUID()).GetString());
        return PACKET_EXE_ERROR;
    }
    BYTE opt = pPacket->GetOpt();
    switch(opt)
    {
        case GWG_HOME_PLANT_OPT_ASK:
        {
            WGHomePlantOpt msgRet;
            msgRet.SetOpt(GWG_HOME_PLANT_OPT_RET_PLANT_INFO);
            if(g_HomePlantManager)
            {
                g_HomePlantManager->FillHomePlatMsg(&msgRet, pPacket->GetGUID());
            }
            g_pServerManager->DirectSendPacket(&msgRet, pUser->GetServerID());
        }
        break;
    }
    return PACKET_EXE_CONTINUE;
    __LEAVE_FUNCTION
    return PACKET_EXE_ERROR;
}
