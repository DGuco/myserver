#include "proxy_player.h"
#include "singleton.h"
#include "proxy_server.h"

CProxyPlayer::CProxyPlayer(CSocket socket)
	:CTCPConn(socket,
				PROXY_SERVER_RECV_BUFF,
				PROXY_SERVER_SEND_BUFF,
				PROXY_SERVER_RECV_BUFF_MAX,
				PROXY_SERVER_SEND_BUFF_MAX)
{
	m_nProxyState = eProInvalid;
	m_nServerType = FE_INVALID;
	m_nServerId   = 0;
}

CProxyPlayer::~CProxyPlayer()
{

}

int CProxyPlayer::DoRecvLogic()
{
	int tmCurPacketLen = m_pReadBuff->CanReadLen();
	//包头前两个字节为数据总长度，如果数据长度小于两个字节返回0
	if (tmCurPacketLen < sizeof(mshead_size))
	{
		return ERR_RECV_OK;
	}
	mshead_size tmPacketLen = m_pReadBuff->ReadT<mshead_size>(true);
	int tmFullPacketLen = tmPacketLen + sizeof(mshead_size);
	//数据不完整
	if (tmFullPacketLen > tmCurPacketLen)
	{
		return ERR_RECV_OK;
	}
	//有完整的数据包，读取处理
	CProxyServer::GetSingletonPtr()->ProcessServerMessage(this);
	return 0;
}

int CProxyPlayer::DoWriteLogic()
{
	return ERR_SEND_OK;
}

int CProxyPlayer::DoClosingLogic(int errcode)
{
	CProxyServer::GetSingletonPtr()->RemoveConnect(this, errcode);
	return 0;
}

void CProxyPlayer::DoTick(time_t now)
{

}

void CProxyPlayer::SetProxyState(short state)
{
	m_nProxyState = state;
}

void CProxyPlayer::SetServerId(int serverid)
{
	m_nServerId = serverid;
}

void CProxyPlayer::SetServerType(enServerType servertype)
{
	m_nServerType = servertype;
}

short CProxyPlayer::GetProxyState()
{
	return m_nProxyState;
}

int CProxyPlayer::GetServerId()
{
	return m_nServerId;
}

enServerType CProxyPlayer::GetServerType()
{
	return m_nServerType;
}

time_t CProxyPlayer::GetLastRecvKeepAlive()
{
	return m_nLastRecvKeepAlive;
}

void CProxyPlayer::SetLastRecvKeepAlive(time_t value)
{
	m_nLastRecvKeepAlive = value;
}

int CProxyPlayer::ConnKey()
{
	return m_nServerId << 8 + m_nServerType;
}

int CProxyPlayer::ConnKey(int servertype, int serverid)
{
	return serverid << 8 + servertype;
}