#include "tcp_conn.h"

CTCPConn::CTCPConn(unsigned int RecvBufLen_, unsigned int SendBufLen_)
	: CTCPSocket(RecvBufLen_, SendBufLen_)
{

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