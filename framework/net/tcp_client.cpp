#include "tcp_client.h"

CTCPClient::CTCPClient(unsigned int RecvBufLen_, unsigned int SendBufLen_)
	: CTCPSocket(RecvBufLen_, SendBufLen_)
{

}

CTCPClient::CTCPClient(CSocket socket, unsigned int RecvBufLen_, unsigned int SendBufLen_)
	: CTCPSocket(socket, RecvBufLen_, SendBufLen_)
{

}

CTCPClient::~CTCPClient()
{

}

void CTCPClient::SetLastSendKeepLive(time_t nKeepAliveTime)
{
	m_nLastSendKeepLive = nKeepAliveTime;
}

time_t CTCPClient::GetLastSendKeepLive()
{
	return m_nLastSendKeepLive;
}