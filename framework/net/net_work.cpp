#include "net_addr.h"
#include "network_interface.h"
#include "net_work.h"
#include "system_signal.h"
#include "connector.h"
#include "acceptor.h"
#include "my_assert.h"

template<> shared_ptr<CNetWork> CSingleton<CNetWork>::spSingleton = NULL;

CNetWork::CNetWork()
	:
	m_pEventReactor(std::make_shared<CEventReactor>()),
	m_uGcTime(0),
	m_pListener(NULL),
	m_pCheckTimerOut(NULL),
	m_iPingCheckTime(0),
	m_pOnNew(NULL),
	m_pFuncAcceptorOnDataSend(NULL),
	m_pFuncAcceptorOnDataRecv(NULL),
	m_pFuncAcceptorDisconnected(NULL)
{
}

CNetWork::~CNetWork(void)
{
	m_mapConnector.clear();
	m_mapAcceptor.clear();
	while (!m_qTimerOrSignals.empty()) {
		m_qTimerOrSignals.pop();
	}
}

bool CNetWork::BeginListen(const char *szNetAddr,
						   unsigned int uPort,
						   FuncAcceptorOnNew pOnNew,
						   FuncBufferEventOnDataSend funcAcceptorOnDataSend,
						   FuncBufferEventOnDataSend funcAcceptorOnDataRecv,
						   FuncBufferEventOnDataSend funcAcceptorDisconnected,
						   FuncOnTimeOut funcAcceptorTimeOut,
						   int listenQueue,
						   unsigned int uCheckPingTickTime)
{
	m_pListener = std::make_shared<CListener>(m_pEventReactor, listenQueue);
	if (!m_pListener) {
		return false;
	}
	CNetAddr addr(szNetAddr, uPort);
	bool bRes = m_pListener->Listen(addr, &CNetWork::lcb_OnAccept);
	m_pOnNew = pOnNew;
	m_pCheckTimerOut = std::make_shared<CTimerEvent>(GetEventReactor(),
													 funcAcceptorTimeOut,
													 this,
													 uCheckPingTickTime / 1000,  //毫秒转换为秒
													 0,
													 -1);
	m_iPingCheckTime = uCheckPingTickTime;
	m_pFuncAcceptorOnDataSend = funcAcceptorOnDataSend;
	m_pFuncAcceptorOnDataRecv = funcAcceptorOnDataRecv;
	m_pFuncAcceptorDisconnected = funcAcceptorDisconnected;
	return bRes;
}

void CNetWork::SetCallBackSignal(unsigned int uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop)
{
	std::shared_ptr<CSystemSignal> pSystemSignal = std::make_shared<CSystemSignal>(m_pEventReactor);
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
	MY_ASSERT_STR(bRet, return, "Acceptor register failed");
	m_pOnNew(socket, pAcceptor);
}

void CNetWork::EndListen()
{
}

bool CNetWork::Connect(const char *szNetAddr,
					   uint16 uPort,
					   int iTargetId,
					   FuncBufferEventOnDataSend funcOnSomeDataSend,
					   FuncBufferEventOnDataSend funcOnSomeDataRecv,
					   FuncBufferEventOnDisconnected funcOnDisconnected,
					   FuncConnectorOnConnectFailed funcOnConnectFailed,
					   FuncConnectorOnConnectted funcOnConnectted,
					   FuncConnectorOnPingServer funcOnPingServer,
					   unsigned int uPingTick)
{
	std::shared_ptr<CConnector> pConnector =
		std::make_shared<CConnector>(m_pEventReactor,
									 funcOnSomeDataSend,
									 funcOnSomeDataRecv,
									 funcOnDisconnected,
									 iTargetId,
									 uPingTick);
	pConnector->SetCallbackFunc(std::move(funcOnConnectFailed),
								std::move(funcOnConnectted),
								std::move(funcOnPingServer));

	CNetAddr addr(szNetAddr, uPort);
	bool bRet = pConnector->Connect(addr);
	if (!bRet) {
		return false;
	}
	InsertNewConnector(iTargetId, pConnector);
	return bRet;
}

void CNetWork::DispatchEvents()
{
	m_pEventReactor->DispatchEvents();
}

bool CNetWork::ShutDownAcceptor(unsigned int uId)
{
	auto iter = m_mapAcceptor.find(uId);
	if (m_mapAcceptor.end() != iter) {
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
		m_mapConnector.erase(iter);
		return true;
	}
	else {
		return false;
	}
}

std::shared_ptr<CConnector> CNetWork::FindConnector(unsigned int uId)
{
	auto iter = m_mapConnector.find(uId);
	if (m_mapConnector.end() == iter) {
		return NULL;
	}
	else {
		return iter->second;
	}
}

std::shared_ptr<CAcceptor> CNetWork::FindAcceptor(unsigned int uId)
{
	auto iter = m_mapAcceptor.find(uId);
	if (m_mapAcceptor.end() == iter) {
		return NULL;
	}
	else {
		return iter->second;
	}
}

void CNetWork::InsertNewAcceptor(unsigned int uid, std::shared_ptr<CAcceptor> pAcceptor)
{
	MY_ASSERT(pAcceptor != NULL, return;)
	m_mapAcceptor.insert(std::make_pair(uid, pAcceptor));
}

void CNetWork::InsertNewConnector(unsigned int uid, std::shared_ptr<CConnector> pConnector)
{
	MY_ASSERT(pConnector != NULL, return;)
	m_mapConnector.insert(std::make_pair(uid, pConnector));
}

std::shared_ptr<IEventReactor> CNetWork::GetEventReactor()
{
	return m_pEventReactor;
}

CNetWork::MAP_ACCEPTOR &CNetWork::GetAcceptorMap()
{
	return m_mapAcceptor;
}