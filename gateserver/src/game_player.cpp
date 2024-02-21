#include "game_player.h"
#include "tcp_socket.h"
#include "client_comm_engine.h"

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

	static CMessage tmpMessage;
	tmpMessage.Clear();
	CMesHead* tmpHead = tmpMessage.mutable_msghead();
	int iTmpRet = CClientCommEngine::ParseClientStream(m_pReadBuff, tmpMessage.mutable_msghead());
	if (iTmpRet < 0) {
		//�Ͽ�����
		return ERR_RECV_PARSE_ERROR;
	}
	time_t tNow = GetMSTime();
	m_nLastRecvKeepLive = tNow;
	//��֯ת����Ϣ
	if (0 == iTmpRet && tmpHead->cmd() != 103) {
		CSocketInfo* tmpSocketInfo = tmpHead->mutable_socketinfos()->Add();
		tmpSocketInfo->Clear();
		tmpSocketInfo->set_createtime(tmpAcceptor->GetCreateTime());
		tmpSocketInfo->set_socketid(tmpAcceptor->GetSocket().GetSocket());
		tmpSocketInfo->set_state(0);
		iTmpRet = CClientCommEngine::ConvertToGameStream(m_pSendBuff.get(),
			m_pRecvBuff->CanReadData(),
			tmpLastLen,
			tmpHead);
		if (iTmpRet != 0) {
			//�Ͽ�����
			ClearSocket(tmpAcceptor, Err_PacketError);
			return;
		}

		shared_ptr<CMessHandle>& tmpServerHandle = CGateCtrl::GetSingletonPtr()->GetMesManager();
		iTmpRet = tmpServerHandle->SendToGame(m_pSendBuff->CanReadData(), m_pSendBuff->ReadableDataLen());
		if (iTmpRet != 0) {
			LOG_ERROR("defalut", "CNetManager::DealClientData to game error, error code {}", iTmpRet);
			ClearSocket(tmpAcceptor, Err_SendToMainSvrd);
		}
		else {
			LOG_DEBUG("default", "gate ==>game succeed");
		}
	}
	else {
		//������Ϣ��������
	}

}

int CGamePlayer::DoWriteLogic()
{
	return ERR_SEND_OK;
}

int CGamePlayer::DoErrorLogic(int errcode)
{

}