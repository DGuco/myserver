#include "gwghomeplantopt.h"

UINT GWGHomePlantOptHandler::Execute(GWGHomePlantOpt* pPacket, Player* pPlayer)
{
    __ENTER_FUNCTION
	ASSERT( pPacket );
    USER* pUser = g_OnlineUser->FindUser(pPacket->GetGUID());
    if(pUser == NULL)
    {
        CacheGuid64Log(LOG_FILE_WORLD, "GWGHomePlantOptHandler::Execute, user not found, guid = %s",GUID64String(pPacket->GetGUID()).GetString());
        return PACKET_EXE_ERROR;
    }
    return PACKET_EXE_CONTINUE;
    __LEAVE_FUNCTION
}
