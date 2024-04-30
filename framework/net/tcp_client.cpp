#include "tcp_client.h"

CTCPClient::CTCPClient(int RecvBufLen_,
					int SendBufLen_,
					int MaxRecvBufLen_,
					int MaxSendBufLen_)
	: CTCPSocket(RecvBufLen_, SendBufLen_, MaxRecvBufLen_, MaxSendBufLen_)
{

}

CTCPClient::~CTCPClient()
{

}

int	CTCPClient::ClientKey()
{
	return GetSocketFD();
}

uint64 CTCPClient::MapKey()
{
	uint64 nSocket = GetSocketFD();
	uint64 nKey = ClientKey();
	return nSocket << 32 + nKey;
}

uint64 CTCPClient::MapKey(int socket, int clientKey)
{
	uint64 nSocket = socket;
	uint64 nKey = clientKey;
	return nSocket << 32 + nKey;
}
