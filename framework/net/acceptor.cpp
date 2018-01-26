#include <my_assert.h>
#include "acceptor.h"

CAcceptor::CAcceptor(SOCKET socket,
                     IEventReactor *pReactor,
                     CNetAddr *netAddr) :
    m_eState(eAS_Disconnected),
    m_uMaxOutBufferSize(POSTBUFLENGTH),
    m_uMaxInBufferSize(RECVBUFLENGTH),
    m_pFuncOnDisconnected(NULL),
    m_pFuncOnSomeDataSend(NULL),
    m_pFuncOnSomeDataRecv(NULL),
    m_pStBufEv(NULL),
    m_pNetAddr(netAddr) {
  m_Socket.SetSystemSocket(socket);
  m_Socket.SetNonblocking();
  m_pReactor->Register(this);
}

CAcceptor::~CAcceptor() {
  Release();
}

void CAcceptor::GetRemoteIpAddress(char *szBuf, uint32 uBufSize) {
  MY_ASSERT_STR(uBufSize >= 16, return, "uBufSize is too small");
  CNetAddr addr;
  m_Socket.GetRemoteAddress(addr);
  strncpy(szBuf, addr.GetAddress(), 16);
}

CAcceptorEx *CAcceptor::GetContext(void) const {
  return m_pContext;
}

void CAcceptor::SetContext(CAcceptorEx *pContext) {
  m_pContext = pContext;
}

void CAcceptor::SetCallbackFunc(FuncAcceptorOnDisconnected pOnDisconnected,
                                FuncAcceptorOnSomeDataSend pOnSomeDataSend,
                                FuncAcceptorOnSomeDataRecv pOnSomeDataRecv) {
  m_pFuncOnDisconnected = pOnDisconnected;
  m_pFuncOnSomeDataSend = pOnSomeDataSend;
  m_pFuncOnSomeDataRecv = pOnSomeDataRecv;
}

IEventReactor *CAcceptor::GetReactor() {
  return m_pReactor;
}

bool CAcceptor::IsConnected() {
  return m_eState == eAS_Connected;
}

bool CAcceptor::RegisterToReactor() {
  m_pStBufEv = bufferevent_socket_new(GetReactor()->GetEventBase(),
                                      -1,
                                      BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);

  MY_ASSERT_STR(NULL != m_pStBufEv, return false, "BufferEvent_new failed!!");
  bufferevent_setcb(m_pStBufEv,
                    CAcceptor::lcb_OnPipeRead,
                    CAcceptor::lcb_OnPipeWrite,
                    CAcceptor::lcb_OnEvent,
                    (void *) this);
  SetState(eAS_Connected);
  return true;
}

void CAcceptor::lcb_OnPipeRead(struct bufferevent *bev, void *arg) {
  CAcceptor *pAcceptor = static_cast<CAcceptor *>(arg);
  pAcceptor->m_pFuncOnSomeDataRecv(pAcceptor);
}

void CAcceptor::lcb_OnPipeWrite(bufferevent *bev, void *arg) {
  CAcceptor *pAcceptor = static_cast<CAcceptor *>(arg);
  pAcceptor->m_pFuncOnSomeDataSend(pAcceptor);
}

void CAcceptor::lcb_OnEvent(bufferevent *bev, int16 nWhat, void *arg) {
  CAcceptor *pAcceptor = static_cast<CAcceptor *>(arg);
  MY_ASSERT_STR(pAcceptor != NULL, return, "CAcceptor Pipe Error with code %d", PpeGetLastError());

  pAcceptor->ShutDown();
  if (nWhat & EVBUFFER_EOF) {
    pAcceptor->m_pFuncOnDisconnected(pAcceptor);
    return;
  } else if (nWhat & EVBUFFER_ERROR) {
    pAcceptor->m_pFuncOnDisconnected(pAcceptor);
    return;
  }

  pAcceptor->ProcessSocketError();
}

void CAcceptor::ShutDown() {
  if (!IsConnected())
    return;

  GH_INFO("ShutDown CAcceptor");

  GetReactor()->UnRegister(this);
  m_Socket.Close();
  SetState(eAS_Disconnected);
}

bool CAcceptor::UnRegisterFromReactor() {
  bufferevent_disable(m_pStBufEv, EV_READ | EV_WRITE);
  return true;
}

void CAcceptor::Release() {
  ShutDown();
  if (m_pStBufEv) {
    bufferevent_free(m_pStBufEv);
    m_pStBufEv = NULL;
  }
  GH_DELETE_T(this, CAcceptor, MEMCATEGORY_GENERAL);
}

PipeResult CAcceptor::Send(const void *pData, uint32 uSize) {
  if (!IsConnected())
    return ePR_Disconnected;

  if (m_pStBufEv->output->off + uSize > m_uMaxOutBufferSize) {
    GH_INFO("CAcceptor send buffer out of size");
    ShutDown();
    m_pFuncOnDisconnected(this);
    return ePR_OutPipeBuf;
  }

  int32 nRes = bufferevent_write(m_pStBufEv, pData, uSize);
  GH_ASSERT(0 == nRes);
  return ePR_OK;
}

PipeResult CAcceptor::Send(const void *pData1, uint32 uSize1, const void *pData2, uint32 uSize2) {
  if (!IsConnected())
    return ePR_Disconnected;

  if (m_pStBufEv->output->off + uSize1 + uSize2 > m_uMaxOutBufferSize) {
    GH_INFO("CAcceptor send buffer out of size");
    ShutDown();
    m_pFuncOnDisconnected(this);
    return ePR_OutPipeBuf;
  }

  int32 nRes = bufferevent_write(m_pStBufEv, pData1, uSize1);
  GH_ASSERT(0 == nRes);

  nRes = bufferevent_write(m_pStBufEv, pData2, uSize2);
  GH_ASSERT(0 == nRes);

  return ePR_OK;
}

PipeResult CAcceptor::Send(const void *pData1, uint32 uSize1, const void *pData2, uint32 uSize2, const void *pData3,
                           uint32 uSize3) {
  if (!IsConnected())
    return ePR_Disconnected;

  if (m_pStBufEv->output->off + uSize1 + uSize2 + uSize3 > m_uMaxOutBufferSize) {
    GH_INFO("CAcceptor send buffer out of size");
    ShutDown();
    m_pFuncOnDisconnected(this);
    return ePR_OutPipeBuf;
  }

  int32 nRes = bufferevent_write(m_pStBufEv, pData1, uSize1);
  GH_ASSERT(0 == nRes);

  nRes = bufferevent_write(m_pStBufEv, pData2, uSize2);
  GH_ASSERT(0 == nRes);

  nRes = bufferevent_write(m_pStBufEv, pData3, uSize3);
  GH_ASSERT(0 == nRes);

  return ePR_OK;
}

void *CAcceptor::GetRecvData() const {
  GH_ASSERT(NULL != m_pStBufEv);
  return m_pStBufEv->input->buffer;
}

uint32 CAcceptor::GetRecvDataSize() {
  GH_ASSERT(NULL != m_pStBufEv);
  return static_cast<uint32>(m_pStBufEv->input->off);
}

uint32 CAcceptor::GetSendDataSize() {
  GH_ASSERT(NULL != m_pStBufEv);
  return static_cast<uint32>(m_pStBufEv->output->off);
}

void CAcceptor::PopRecvData(uint32 uSize) {
  GH_ASSERT(NULL != m_pStBufEv);
  GH_ASSERT(m_pStBufEv->input->off >= uSize);
  GH_ASSERT(uSize >= 0);
  if (uSize > 0)
    evbuffer_drain(m_pStBufEv->input, uSize);
}

void CAcceptor::SetMaxSendBufSize(uint32 uSize) {
  GH_ASSERT(uSize > 0);
  m_uMaxOutBufferSize = uSize;
  if (IsConnected()) {
    GH_ASSERT(NULL != m_pStBufEv);
    bufferevent_setwatermark(m_pStBufEv, EV_WRITE, 0, m_uMaxOutBufferSize);
  }
}

uint32 CAcceptor::GetMaxSendBufSize() {
  return m_uMaxOutBufferSize;
}

void CAcceptor::SetMaxRecvBufSize(uint32 uSize) {
  GH_ASSERT(uSize > 0);
  m_uMaxInBufferSize = uSize;
  if (IsConnected()) {
    GH_ASSERT(NULL != m_pStBufEv);
    bufferevent_setwatermark(m_pStBufEv, EV_READ, 0, m_uMaxInBufferSize);
  }
}

uint32 CAcceptor::GetMaxRecvBufSize() {
  return m_uMaxInBufferSize;
}

CAcceptor::eAcceptorState CAcceptor::GetState() {
  return m_eState;
}

void CAcceptor::SetState(eAcceptorState eState) {
  m_eState = eState;
}

CSocket CAcceptor::GetSocket() const {
  return m_Socket;
}

void CAcceptor::ProcessSocketError() {
  switch (m_Socket.GetSocketError()) {
    case ePCFR_UNREACH:
    case ePCFR_REFUSED:
    case ePCFR_RESET:
    case ePCFR_LOCALADDRINUSE:
    case ePCFR_NOBUFFER:
    case ePCFR_TIMEDOUT: {
      m_pFuncOnDisconnected(this);
      break;
    }
    default: MY_ASSERT_STR(false, return, "Unknown socket error");
  }
}