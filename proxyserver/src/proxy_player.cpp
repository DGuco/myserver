#include "proxy_player.h"
#include "server_tool.h"
#include "common_def.h"
#include "proxy_server.h"

CProxyPlayer::CProxyPlayer() 
	:CTCPConn(GAMEPLAYER_RECV_BUFF_LEN, GAMEPLAYER_SEND_BUFF_LEN)
{

}

int CProxyPlayer::DoRecvLogic()
{
	unsigned short tmCurPacketLen = m_pReadBuff->CanReadLen();
	//包头前两个字节为数据总长度，如果数据长度小于两个字节返回0
	if (tmCurPacketLen < sizeof(msize_t))
	{
		return ERR_RECV_OK;
	}
	msize_t tmPacketLen = m_pReadBuff->ReadUnInt(true);
	msize_t tmFullPacketLen = tmPacketLen + sizeof(msize_t);
	//数据不完整
	if (tmFullPacketLen > tmCurPacketLen)
	{
		return ERR_RECV_OK;
	}
	//有完整的数据包，读取处理
	CProxyServer::GetSingletonPtr()->RecvClientData(this);
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