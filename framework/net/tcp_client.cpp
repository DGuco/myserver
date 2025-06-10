#include "tcp_client.h"
#include "time_helper.h"
#include "log.h"
#include "message.pb.h"

CTCPClient::CTCPClient(int RecvBufLen_,
					int SendBufLen_,
					int MaxRecvBufLen_,
					int MaxSendBufLen_,
				    int sendHeartbeatCD_)
	: CTCPSocket(RecvBufLen_, SendBufLen_, MaxRecvBufLen_, MaxSendBufLen_)
{
	m_nSendHeartbeatCD = sendHeartbeatCD_;
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

// int CTCPClient::SendHeartbeat()
// {
// 	if(IsValid() == false)
// 	{
// 		return ERR_SOCKE_NOSOCK;
// 	}
// 	// time_t nNowAnsTime = CTimeHelper::GetSingletonPtr()->GetMSTime();
// 	// if(nNowAnsTime - m_nLastSendHeartbeatTime >= m_nSendHeartbeatCD)
// 	// {
// 	// 	m_nLastSendHeartbeatTime = nNowAnsTime;
// 	// 	HeartBeatMsg msg;
// 	// 	msg.set_beattype(enHeartBeatType::MESS_HEARTBEAT);
// 	// 	bool bRet = msg.SerializePartialToArray(CTCPClient::m_CacheData, MAX_PACKAGE_LEN);
// 	// 	if (!bRet)
// 	// 	{
// 	// 		CACHE_LOG(TCP_ERROR, "CTCPClient::SendHeartbeat SerializePartialToArray failed");
// 	// 		return ERR_SOCKE_SERIAL_ERROR;
// 	// 	}
// 	// 	int nCode = Write(m_CacheData, msg.GetCachedSize());
// 	// 	if (nCode != ERR_SOCKE_OK)
// 	// 	{
// 	// 		CACHE_LOG(TCP_ERROR, "CTCPClient::SendHeartbeat Write failed£¬ip:{} port:{},code = {} errormsg = {}", 
// 	// 			m_Socket.GetHost(), m_Socket.GetPort(),nCode,strerror(socket_error));
// 	// 	}
// 	// }

// 	HeartBeatMsg msg;
// 	msg.set_beattype(enHeartBeatType::MESS_HEARTBEAT);
// 	bool bRet = msg.SerializePartialToArray(CTCPClient::m_CacheData, MAX_PACKAGE_LEN);
// 	if (!bRet)
// 	{
// 		return ERR_SOCKE_SERIAL_ERROR;
// 	}
// 	return Write(m_CacheData, msg.GetCachedSize());
// }