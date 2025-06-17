#include "proxy_conn.h"
#include "singleton.h"
#include "proxy_server.h"

CProxyConn::CProxyConn(CSocket socket)
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

CProxyConn::~CProxyConn()
{

}

int CProxyConn::DoRecvLogic()
{
	int tmCurPacketLen = m_pReadBuff->CanReadLen();
	//包头前两个字节为数据总长度，如果数据长度小于两个字节返回0
	if (tmCurPacketLen < sizeof(mshead_size))
	{
		return ERR_SOCKE_OK;
	}
	mshead_size tmPacketLen = m_pReadBuff->ReadT<mshead_size>(true);
	int tmFullPacketLen = tmPacketLen + sizeof(mshead_size);
	//数据不完整
	if (tmFullPacketLen > tmCurPacketLen)
	{
		return ERR_SOCKE_OK;
	}
	//有完整的数据包，读取处理
	CProxyServer::GetSingletonPtr()->ProcessServerMessage(this);
	return 0;
}

int CProxyConn::DoWriteLogic()
{
	return ERR_SOCKE_OK;
}

void CProxyConn::DoTick(time_t now)
{

}

void CProxyConn::SetProxyState(short state)
{
	m_nProxyState = state;
}

void CProxyConn::SetServerId(int serverid)
{
	m_nServerId = serverid;
}

void CProxyConn::SetServerType(enServerType servertype)
{
	m_nServerType = servertype;
}

short CProxyConn::GetProxyState()
{
	return m_nProxyState;
}

int CProxyConn::GetServerId()
{
	return m_nServerId;
}

enServerType CProxyConn::GetServerType()
{
	return m_nServerType;
}

int CProxyConn::ConnKey()
{
	return (m_nServerId << 8) + m_nServerType;
}

int CProxyConn::ConnKey(int servertype, int serverid)
{
	return (serverid << 8) + servertype;
}