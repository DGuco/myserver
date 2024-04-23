#include "tcp_client.h"

CTCPClient::CTCPClient(unsigned int RecvBufLen_, unsigned int SendBufLen_)
	: CTCPSocket(RecvBufLen_, SendBufLen_)
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
