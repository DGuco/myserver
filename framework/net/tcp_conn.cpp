#include "tcp_conn.h"

CTCPConn::CTCPConn(CSocket socket, unsigned int RecvBufLen_, unsigned int SendBufLen_)
	: CTCPSocket(socket, RecvBufLen_, SendBufLen_)
{
}

CTCPConn::~CTCPConn()
{

}