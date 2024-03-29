#include "proxy_player.h"
#include "singleton.h"
#include "common_def.h"
#include "proxy_server.h"

CProxyPlayer::CProxyPlayer(CSocket socket)
	:CTCPConn(socket,GAMEPLAYER_RECV_BUFF_LEN, GAMEPLAYER_SEND_BUFF_LEN)
{

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
	CProxyServer::GetSingletonPtr()->RecvClientData(this);
	return 0;
}

int CProxyPlayer::DoWriteLogic()
{
	return ERR_SEND_OK;
}

int CProxyPlayer::DoErrorLogic(int errcode)
{
	CProxyServer::GetSingletonPtr()->DisConnect(this, errcode);
	return 0;
}

void CProxyPlayer::DoTick(time_t now)
{

}