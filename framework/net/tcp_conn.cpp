#include "tcp_conn.h"

CTCPConn::CTCPConn(CSocket socket, 
				int RecvBufLen_,
				int SendBufLen_,
				int MaxRecvBufLen_,
				int MaxSendBufLen_)
	: CTCPSocket(socket, RecvBufLen_, SendBufLen_, MaxRecvBufLen_, MaxSendBufLen_)
{
}

CTCPConn::~CTCPConn()
{

}