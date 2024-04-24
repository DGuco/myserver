#include "proxy_player.h"
#include "singleton.h"
#include "common_def.h"
#include "proxy_server.h"

CProxyPlayer::CProxyPlayer(CSocket socket)
	:CTCPConn(socket,GAMEPLAYER_RECV_BUFF_LEN, GAMEPLAYER_SEND_BUFF_LEN)
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
	msize_t tmCurPacketLen = m_pReadBuff->CanReadLen();
	//��ͷǰ�����ֽ�Ϊ�����ܳ��ȣ�������ݳ���С�������ֽڷ���0
	if (tmCurPacketLen < sizeof(msize_t))
	{
		return ERR_RECV_OK;
	}
	msize_t tmPacketLen = m_pReadBuff->ReadUnInt(true);
	msize_t tmFullPacketLen = tmPacketLen + sizeof(msize_t);
	//���ݲ�����
	if (tmFullPacketLen > tmCurPacketLen)
	{
		return ERR_RECV_OK;
	}
	//�����������ݰ�����ȡ����
	CProxyServer::GetSingletonPtr()->ProcessServerMessage(this);
	return 0;
}

int CProxyPlayer::DoWriteLogic()
{
	return ERR_SEND_OK;
}

int CProxyPlayer::DoClosingLogic(int errcode)
{
	CProxyServer::GetSingletonPtr()->DisConnect(this, errcode);
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