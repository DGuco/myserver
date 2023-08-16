#include "tcp_client.h"

CTCPClient::CTCPClient(unsigned int RecvBufLen_, unsigned int SendBufLen_) 
	: CTCPSocket(RecvBufLen_, SendBufLen_)
{

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