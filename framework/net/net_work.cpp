#include <my_macro.h>
#include "net_addr.h"
#include "network_interface.h"
#include "net_work.h"
#include "system_signal.h"
#include "connector.h"
#include "acceptor.h"
#include "my_assert.h"

template<> CNetWork *CSingleton<CNetWork>::spSingleton = NULL;

CNetWork::CNetWork()
	:
	m_pEventReactor(new CEventReactor()),
	m_uGcTime(0),
	m_uCheckPingTickTime(0),
	m_pListener(NULL),
	m_pOnNew(NULL),
	m_pFuncAcceptorOnDataSend(NULL),
	m_pFuncAcceptorOnDataRecv(NULL),
	m_pFuncAcceptorDisconnected(NULL)
{
}

CNetWork::~CNetWork(void)
{
	for (auto it : m_mapConnector) {
		SAFE_DELETE(it.second);
	}
	m_mapConnector.clear();

	for (auto it : m_mapAcceptor) {
		SAFE_DELETE(it.second);
	}
	m_mapAcceptor.clear();
	while (!m_qTimerOrSignals.empty()) {
		CSystemSignal *pSystemSignal = m_qTimerOrSignals.front();
		SAFE_DELETE(pSystemSignal);
		m_qTimerOrSignals.pop();
	}
	OnTick();
	SAFE_DELETE(m_pEventReactor);
}

void CNetWork::SetCallBackSignal(unsigned int uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop)
{
	CSystemSignal *pSystemSignal = new CSystemSignal(m_pEventReactor);
	pSystemSignal->SetCallBackSignal(uSignal, pFunc, pContext, bLoop);
	m_qTimerOrSignals.push(pSystemSignal);
	m_qTimerOrSignals.push(pSystemSignal);
}

void CNetWork::lcb_OnAccept(IEventReactor *pReactor, SOCKET socket, sockaddr *sa)
{
	CNetWork::GetSingletonPtr()->NewAcceptor(pReactor, socket, sa);
}

void CNetWork::NewAcceptor(IEventReactor *pReactor, SOCKET socket, sockaddr *sa)
{
	sockaddr_in sin;
	memcpy(&sin, sa, sizeof(sin));
	//  取得ip和端口号
	char ip[16];
	sprintf(ip, inet_ntoa(sin.sin_addr));
	CAcceptor *pAcceptor = new CAcceptor(socket,
										 pReactor,
										 new CNetAddr(ip, sin.sin_port),
										 m_pFuncAcceptorOnDataSend,
										 m_pFuncAcceptorOnDataRecv,
										 m_pFuncAcceptorDisconnected);
	MY_ASSERT_STR(pAcceptor != NULL, return, "Create CAcceptor failed");
	bool bRet = GetEventReactor()->Register(pAcceptor);
	MY_ASSERT_STR(bRet, return, "Create CAcceptor failed");
	m_pOnNew(socket, pAcceptor);
}

bool CNetWork::BeginListen(const char *szNetAddr,
						   unsigned int uPort,
						   FuncAcceptorOnNew pOnNew,
						   FuncBufferEventOnDataSend funcAcceptorOnDataSend,
						   FuncBufferEventOnDataSend funcAcceptorOnDataRecv,
						   FuncBufferEventOnDataSend funcAcceptorDisconnected,
						   int listenQueue,
						   unsigned int uCheckPingTickTime)
{
	m_pListener = new CListener(m_pEventReactor, listenQueue);
	if (m_pListener == NULL) {
		return false;
	}
	CNetAddr addr(szNetAddr, uPort);

	bool bRes = m_pListener->Listen(addr, &CNetWork::lcb_OnAccept);
	m_uCheckPingTickTime = uCheckPingTickTime;
	m_pOnNew = pOnNew;
	m_pFuncAcceptorOnDataSend = funcAcceptorOnDataSend;
	m_pFuncAcceptorOnDataRecv = funcAcceptorOnDataRecv;
	m_pFuncAcceptorDisconnected = funcAcceptorDisconnected;
	return bRes;
}

void CNetWork::EndListen()
{
	SAFE_DELETE(m_pListener);
}

bool CNetWork::Connect(const char *szNetAddr,
					   uint16 uPort,
					   FuncBufferEventOnDataSend funcOnSomeDataSend,
					   FuncBufferEventOnDataSend funcOnSomeDataRecv,
					   FuncBufferEventOnDisconnected funcOnDisconnected,
					   FuncConnectorOnConnectFailed funcOnConnectFailed,
					   FuncConnectorOnConnectted funcOnConnectted,
					   FuncConnectorOnPingServer funcOnPingServer,
					   unsigned int uPingTick)
{
	CConnector *pConnector = new CConnector(m_pEventReactor,
											funcOnSomeDataSend,
											funcOnSomeDataRecv,
											funcOnDisconnected);
	pConnector->SetCallbackFunc(std::move(funcOnConnectFailed),
								std::move(funcOnConnectted));

	CNetAddr addr(szNetAddr, uPort);
	bool bRet = pConnector->Connect(addr);
	if (!bRet) {
		return false;
	}
	int fd = pConnector->GetSocket().GetSocket();
	m_mapConnector.insert(std::make_pair(fd, pConnector));
	return bRet;
}

unsigned int CNetWork::GetConnectorExPingValue(unsigned int uId)
{
	IBufferEvent *pConnector = FindConnector(uId);
	if (pConnector) {
//		return pConnector->GetPingValue();
	}
	else {
		return 9999999;
	}
	return 0;
}

void CNetWork::DispatchEvents()
{
	m_pEventReactor->DispatchEvents();
}

bool CNetWork::ShutDownAcceptor(unsigned int uId)
{
	auto iter = m_mapAcceptor.find(uId);
	if (m_mapAcceptor.end() != iter) {
		IBufferEvent *pAcceptor = iter->second;
		SAFE_DELETE(pAcceptor);
		m_mapAcceptor.erase(iter);
		return true;
	}
	else {
		return false;
	}
}

bool CNetWork::ShutDownConnectorEx(unsigned int uId)
{
	auto iter = m_mapConnector.find(uId);
	if (m_mapConnector.end() != iter) {
		IBufferEvent *pConnector = iter->second;
		SAFE_DELETE(pConnector);
		m_quIdleConnectorExs.push(pConnector);
		m_mapConnector.erase(iter);
		return true;
	}
	else {
		return false;
	}
}

void CNetWork::OnTick()
{
	if (!m_quIdleConnectorExs.empty()) {
		while (!m_quIdleConnectorExs.empty()) {
			IBufferEvent *pConnector = m_quIdleConnectorExs.front();
			SAFE_DELETE(pConnector)
			m_quIdleConnectorExs.pop();
		}
	}
}

IBufferEvent *CNetWork::FindConnector(unsigned int uId)
{
	auto iter = m_mapConnector.find(uId);
	if (m_mapConnector.end() == iter) {
		return NULL;
	}
	else {
		return iter->second;
	}
}

IBufferEvent *CNetWork::FindAcceptor(unsigned int uId)
{
	auto iter = m_mapAcceptor.find(uId);
	if (m_mapAcceptor.end() == iter) {
		return NULL;
	}
	else {
		return iter->second;
	}
}

void CNetWork::InsertNewAcceptor(unsigned int socket, IBufferEvent *pCAcceptor)
{
	MY_ASSERT(pCAcceptor != NULL, return;)
	m_mapAcceptor.insert(std::make_pair(socket, pCAcceptor));
}

IEventReactor *CNetWork::GetEventReactor()
{
	return m_pEventReactor;
}

int CNetWork::ConnectorSendData(unsigned int uId, const void *pData, unsigned int uSize)
{
	IBufferEvent *pConnectorEx = FindConnector(uId);
	if (pConnectorEx) {
		int eRes = pConnectorEx->Send(pData, uSize);
		return eRes;
	}
	return ePR_Disconnected;
}

