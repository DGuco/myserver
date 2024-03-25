#include "server_client.h"
#include "common_def.h"


CServerClient::CServerClient()
	:CTCPClient(GAMEPLAYER_RECV_BUFF_LEN, GAMEPLAYER_SEND_BUFF_LEN)
{}

int CServerClient::DoRecvLogic()
{
	return 0;
}

int CServerClient::DoWriteLogic()
{
	return ERR_SEND_OK;
}

int CServerClient::DoErrorLogic(int errcode)
{
	return 0;
}