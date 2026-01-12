#include "wghomeplantopt.h"
#include "homeplant_manager.h"

UINT WGHomePlantOptHandler::Execute(WGHomePlantOpt* pPacket, Player* pPlayer)
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
    }
    return PACKET_EXE_CONTINUE;
    __LEAVE_FUNCTION
    return PACKET_EXE_ERROR;
}
