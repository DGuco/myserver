#include "tcp_client.h"

CTCPClient::CTCPClient(CSocket tmSocket, int RecvBufLen_, unsigned int SendBufLen_)
	: CTCPSocket(RecvBufLen_, SendBufLen_)
{
	m_Socket = tmSocket;
}

CTCPClient::~CTCPClient()
{

}

int CTCPClient::DoRecvLogic()
{

}
int CTCPClient::DoWriteLogic()
{

}

int CTCPClient::DoFdErrorLogic()
{

}