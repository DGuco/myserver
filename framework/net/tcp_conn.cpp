#include "tcp_conn.h"

CTCPConn::CTCPConn(unsigned int RecvBufLen_, unsigned int SendBufLen_)
	: CTCPSocket(RecvBufLen_, SendBufLen_)
{
	m_nLastRecvKeepLive = 0;
	m_nCreateTime = 0;
}

CTCPConn::CTCPConn(CSocket socket, unsigned int RecvBufLen_, unsigned int SendBufLen_)
	: CTCPSocket(socket, RecvBufLen_, SendBufLen_)
{
	m_nLastRecvKeepLive = 0;
	m_nCreateTime = 0;
}

CTCPConn::~CTCPConn()
{

}

void CTCPConn::SetLastRecvKeepLive(time_t nKeepAliveTime)
{
	m_nLastRecvKeepLive = nKeepAliveTime;
}

time_t CTCPConn::GetLastRecvKeepLive()
{
	return m_nLastRecvKeepLive;
}

void CTCPConn::SetCreateTime(time_t nCreateTime)
{
	m_nCreateTime = nCreateTime;
}

time_t  CTCPConn::GetCreateTime()
{
	return m_nCreateTime;
}