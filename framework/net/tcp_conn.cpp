#include "tcp_conn.h"
#include "message.pb.h"

CTCPConn::CTCPConn(CSocket socket, 
				int RecvBufLen_,
				int SendBufLen_,
				int MaxRecvBufLen_,
				int MaxSendBufLen_)
	: CTCPSocket(socket, RecvBufLen_, SendBufLen_, MaxRecvBufLen_, MaxSendBufLen_)
{
	m_nLastRecvHeartbeatTime = 0;
}

CTCPConn::~CTCPConn()
{

}

// int CTCPConn::SendHeartbeatCallBack()
// {
// 	if(!IsValid())
// 	{	
// 		return ERR_SOCKE_NOSOCK;
// 	}
// 	HeartBeatMsg msg;
// 	msg.set_beattype(enHeartBeatType::MESS_HEARTBEAT_CALLBACK);
// 	bool bRet = msg.SerializePartialToArray(CTCPConn::m_CacheData, MAX_PACKAGE_LEN);
// 	if (!bRet)
// 	{
// 		return ERR_SOCKE_SERIAL_ERROR;
// 	}
// 	return Write(m_CacheData, msg.GetCachedSize());
// }