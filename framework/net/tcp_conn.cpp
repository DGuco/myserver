#include "tcp_conn.h"

CTCPConn::CTCPConn(CSocket tmSocket, int RecvBufLen_, unsigned int SendBufLen_)
	: CTCPSocket(RecvBufLen_, SendBufLen_)
{
	m_Socket = tmSocket;
}

CTCPConn::~CTCPConn()
{

}

int CTCPConn::Key()
{

}

int CTCPConn::DoRecvLogic()
{

}
int CTCPConn::DoWriteLogic()
{

}

int CTCPConn::DoFdErrorLogic()
{

}