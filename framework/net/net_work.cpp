#include <my_macro.h>
#include "net_addr.h"
#include "network_interface.h"

#include "connectorex.h"
#include "acceptorex.h"
#include "net_work.h"
#include "ppe_signal.h"
#include "listener.h"
#include "connector.h"

template<> CNetWork *CSingleton<CNetWork>::spSingleton = NULL;

CNetWork::CNetWork() :
	m_pEventReactor(NULL),
	m_uGcTime(0),
	m_uCheckPingTickTime(0),
	m_pListener(NULL),
	m_pOnNew(NULL),
	m_pOnDisconnected(NULL),
	m_pOnSomeDataSend(NULL),
	m_pOnSomeDataRecv(NULL) {
}

CNetWork::~CNetWork(void) {

}

void CNetWork::Init(eNetModule netModule) {
  m_pEventReactor = new CEventReactor(netModule);
}

void CNetWork::Release(void) {
  for (auto it : m_mapConnectorExs) {
	it.second->ShutDown();
	SAFE_DELETE(it.second);
  }
  m_mapConnectorExs.clear();

  for (auto it : m_mapAcceptorExs) {
	it.second->ShutDown();
	SAFE_DELETE(it.second);
  }
  m_mapAcceptorExs.clear();
  while (!m_quSystemSignals.empty()) {
	CSystemSignal *pSystemSignal = m_quSystemSignals.front();
	SAFE_DELETE(pSystemSignal);
	m_quSystemSignals.pop();
  }
  OnTick();
  SAFE_DELETE(m_pEventReactor);
}

void CNetWork::SetCallBackSignal(uint32 uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop) {
  CSystemSignal *pSystemSignal = new CSystemSignal(m_pEventReactor);
  pSystemSignal->SetCallBackSignal(uSignal, pFunc, pContext, bLoop);
  m_quSystemSignals.push(pSystemSignal);
}

void CNetWork::OnAccept(IEventReactor *pReactor, SOCKET Socket, sockaddr *sa) {
  CNetWork::GetSingletonPtr()->NewAcceptor(pReactor, Socket, sa);
}

void CNetWork::NewAcceptor(IEventReactor *pReactor, SOCKET Socket, sockaddr *sa) {
  sockaddr_in sin;
  memcpy(&sin, sa, sizeof(sin));
  //  取得ip和端口号
  char *ip;
  sprintf(ip, inet_ntoa(sin.sin_addr));
  CAcceptor *pAcceptor = new CAcceptor(Socket, pReactor, new CNetAddr(ip, sin.sin_port));
  SOCKET socket = pAcceptor->GetSocket().GetSocket();
  CAcceptorEx *pAcceptorEx = new CAcceptorEx(socket, pAcceptor);
  pAcceptorEx->SetCallbackFunc(m_pOnDisconnected,
							   m_pOnSomeDataSend,
							   m_pOnSomeDataRecv,
							   m_uCheckPingTickTime);
  m_mapAcceptorExs.insert(std::make_pair(socket, pAcceptorEx));
  m_pOnNew(socket, pAcceptorEx);
}

bool CNetWork::BeginListen(const char *szNetAddr, uint16 uPort,
						   FuncAcceptorExOnNew pOnNew,
						   FuncAcceptorExOnDisconnected pOnDisconnected,
						   FuncAcceptorExOnSomeDataSend pOnSomeDataSend,
						   FuncAcceptorExOnSomeDataRecv pOnSomeDataRecv,
						   uint32 uCheckPingTickTime) {
  m_pListener = new CListener(m_pEventReactor);
  if (m_pListener == NULL) {
	return false;
  }
  CNetAddr addr(szNetAddr, uPort);

  bool bRes = m_pListener->Listen(addr, &CNetWork::OnAccept);
  m_uCheckPingTickTime = uCheckPingTickTime;
  m_pOnNew = pOnNew;
  m_pOnDisconnected = pOnDisconnected;
  m_pOnSomeDataSend = pOnSomeDataSend;
  m_pOnSomeDataRecv = pOnSomeDataRecv;

  return bRes;
}

void CNetWork::EndListen() {
  SAFE_DELETE(m_pListener);
}

uint32 CNetWork::Connect(const char *szNetAddr,
						 uint16 uPort,
						 FuncConnectorExOnDisconnected pOnDisconnected,
						 FuncConnectorExOnConnectFailed pOnConnectFailed,
						 FuncConnectorExOnConnectted pOnConnectted,
						 FuncConnectorExOnSomeDataSend pOnSomeDataSend,
						 FuncConnectorExOnSomeDataRecv pOnSomeDataRecv,
						 FuncConnectorExOnPingServer pOnPingServer,
						 uint32 uPingTick /* = 45000 */, uint32 uTimeOut) {
  CConnector *pConnector = new CConnector(m_pEventReactor);
  CConnectorEx *pConnectorEx = new CConnectorEx(uId, pConnector);
  pConnectorEx->SetCallbackFunc(pOnDisconnected, pOnConnectFailed,
								pOnConnectted, pOnSomeDataSend,
								pOnSomeDataRecv, pOnPingServer,
								uPingTick);

  CNetAddr addr(szNetAddr, uPort);
  timeval time;
  time.tv_sec = uTimeOut;
  time.tv_usec = 0;
  pConnector->Connect(addr, &time);
  m_mapConnectorExs.insert(std::make_pair(uId, pConnectorEx));
  return uId;
}

uint32 CNetWork::GetConnectorExPingValue(uint32 uId) {
  CConnectorEx *pEx = FindConnectorEx(uId);
  if (pEx) {
	return pEx->GetPingValue();
  } else {
	return 9999999;
  }
}

void CNetWork::DispatchEvents() {
  m_pEventReactor->DispatchEvents();
}

bool CNetWork::ShutDownConnectorEx(uint32 uId) {
  Map_ConnectorExs::iterator iter = m_mapConnectorExs.find(uId);

  if (m_mapConnectorExs.end() != iter) {
	CConnectorEx *pConnectorEx = iter->second;

	pConnectorEx->ShutDown();

	m_quIdleConnectorExs.push(pConnectorEx);

	m_mapConnectorExs.erase(iter);
	return true;
  } else {
	return false;
  }
}

void CNetWork::OnTick() {
  if (!m_quIdleConnectorExs.empty()) {
	while (!m_quIdleConnectorExs.empty()) {
	  CConnectorEx *pConnectorEx = m_quIdleConnectorExs.front();
	  pConnectorEx->Release();
	  m_quIdleConnectorExs.pop();
	}
  }
}

CConnectorEx *CNetWork::FindConnectorEx(unsigned int uId) {
  auto iter = m_mapConnectorExs.find(uId);
  if (m_mapConnectorExs.end() == iter) {
	return NULL;
  } else {
	return iter->second;
  }
}

PipeResult CNetWork::ConnectorExSendData(uint32 uId, const void *pData, uint32 uSize) {
  CConnectorEx *pConnectorEx = FindConnectorEx(uId);
  if (pConnectorEx) {
	PipeResult eRes = pConnectorEx->Send(pData, uSize);
	GH_ASSERT(ePR_OK == eRes);
	return ePR_OK;
  }
  return ePR_Disconnected;
}