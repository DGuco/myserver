#include "game_player.h"
#include "gate_server.h"
#include "server_tool.h"
#include "common_def.h"

CGamePlayer::CGamePlayer() 
	:CTCPConn(GAMEPLAYER_RECV_BUFF_LEN, GAMEPLAYER_SEND_BUFF_LEN)
{

}

int CGamePlayer::DoRecvLogic()
{
	unsigned short tmCurPacketLen = m_pReadBuff->CanReadLen();
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
	CGateServer::GetSingletonPtr()->RecvClientData(this);
}

int CGamePlayer::DoWriteLogic()
{
	return ERR_SEND_OK;
}

int CGamePlayer::DoErrorLogic(int errcode)
{
	CGateServer::GetSingletonPtr()->DisConnect(this, errcode);
	return 0;
}