#include <my_macro.h>
#include "net_addr.h"
#include "network_interface.h"
#include "net_work.h"
#include "system_signal.h"
#include "connector.h"
#include "acceptor.h"

template<> CNetWork *CSingleton<CNetWork>::spSingleton = NULL;

CNetWork::CNetWork(eNetModule netModule)
	:
	m_pEventReactor(new CEventReactor(netModule)),
	m_uGcTime(0),
	m_uCheckPingTickTime(0),
	m_pListener(NULL),
	m_pOnNew(NULL),
	m_pOnDisconnected(NULL),
	m_pOnSomeDataRecv(NULL)
{
}

CNetWork::~CNetWork(void)
{
	for (auto it : m_mapConnector) {
		it.second->ShutDown();
		SAFE_DELETE(it.second);
	}
	m_mapConnector.clear();

	for (auto it : m_mapAcceptor) {
		it.second->ShutDown();
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
	CAcceptor *pAcceptor =
		new CAcceptor(socket, pReactor, new CNetAddr(ip, sin.sin_port), m_pOnDisconnected, m_pOnSomeDataRecv);
	MY_ASSERT_STR(pAcceptor != NULL, return, "Create CAcceptor failed");
	bool bRet = GetEventReactor()->Register(pAcceptor);
	MY_ASSERT_STR(bRet, return, "Create CAcceptor failed");
	m_pOnNew(socket, pAcceptor);
}

bool CNetWork::BeginListen(const char *szNetAddr, unsigned int uPort,
						   FuncAcceptorOnNew pOnNew,
						   FuncAcceptorOnDisconnected pOnDisconnected,
						   FuncAcceptorOnSomeDataRecv pOnSomeDataRecv,
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
	m_pOnDisconnected = pOnDisconnected;
	m_pOnSomeDataRecv = pOnSomeDataRecv;

	return bRes;
}

void CNetWork::EndListen()
{
	SAFE_DELETE(m_pListener);
}

bool CNetWork::Connect(const char *szNetAddr,
					   uint16 uPort,
					   FuncConnectorOnDisconnected pOnDisconnected,
					   FuncConnectorOnConnectFailed pOnConnectFailed,
					   FuncConnectorOnConnectted pOnConnectted,
					   FuncConnectorOnSomeDataSend pOnSomeDataSend,
					   FuncConnectorOnSomeDataRecv pOnSomeDataRecv,
					   FuncConnectorOnPingServer pOnPingServer,
					   unsigned int uPingTick /* = 45000 */, unsigned int uTimeOut)
{
	CConnector *pConnector = new CConnector(m_pEventReactor);
	pConnector->SetCallbackFunc(std::move(pOnDisconnected),
								std::move(pOnConnectFailed),
								std::move(pOnConnectted),
								std::move(pOnSomeDataSend),
								std::move(pOnSomeDataRecv));

	CNetAddr addr(szNetAddr, uPort);
	timeval time;
	time.tv_sec = uTimeOut;
	time.tv_usec = 0;
	bool bRet = pConnector->Connect(addr, &time);
	int fd = pConnector->GetSocket().GetSocket();
	{
		m_mapConnector.insert(std::make_pair(fd, pConnector));
	}
	return bRet;
}

unsigned int CNetWork::GetConnectorExPingValue(unsigned int uId)
{
	CConnector *pConnector = FindConnector(uId);
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
		CAcceptor *pAcceptor = iter->second;
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
		CConnector *pConnector = iter->second;
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
			CConnector *pConnector = m_quIdleConnectorExs.front();
			SAFE_DELETE(pConnector)
			m_quIdleConnectorExs.pop();
		}
	}
}

CConnector *CNetWork::FindConnector(unsigned int uId)
{
	auto iter = m_mapConnector.find(uId);
	if (m_mapConnector.end() == iter) {
		return NULL;
	}
	else {
		return iter->second;
	}
}

CAcceptor *CNetWork::FindAcceptor(unsigned int uId)
{
	auto iter = m_mapAcceptor.find(uId);
	if (m_mapAcceptor.end() == iter) {
		return NULL;
	}
	else {
		return iter->second;
	}
}

void CNetWork::InsertNewAcceptor(unsigned int socket, CAcceptor *pCAcceptor)
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
	CConnector *pConnectorEx = FindConnector(uId);
	if (pConnectorEx) {
		int eRes = pConnectorEx->Send(pData, uSize);
		return eRes;
	}
	return ePR_Disconnected;
}

