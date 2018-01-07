#include "acceptorex.h"
#include "Timer.h"

namespace Net
{
	CAcceptorEx::CAcceptorEx(uint32 uId, IAcceptor* pAcceptor)
		:m_uId(uId)
		,m_pAcceptor(pAcceptor)
		,m_uCheckClientTick(0)
		,m_uClientLastPingTime(0)
		,m_uDecodeKey(0x00)
		,m_uEncodeKey(0x00)
	{
		m_pAcceptor->SetContext(this);
		m_pAcceptor->SetCallbackFunc(CbOnDisconnected, CbOnSomeDataSend, CbOnSomeDataRecv);
	}

	CAcceptorEx::~CAcceptorEx(void)
	{
	}


	uint32 CAcceptorEx::GetId() const
	{
		return m_uId;
	}

	void CAcceptorEx::GetRemoteIpAddress(char* szBuf, uint32 uBufSize)
	{
		m_pAcceptor->GetRemoteIpAddress(szBuf, uBufSize);
	}

	void CAcceptorEx::ShutDown()
	{
		if(m_uCheckClientTick)
		{
			GameHub::GetTickMgr()->UnRegisterTick(this);
			m_uCheckClientTick = 0;
		}
		
		m_pAcceptor->ShutDown();
	}

	void CAcceptorEx::Release()
	{
		m_pAcceptor->ShutDown();
		m_pAcceptor->Release();
		m_pAcceptor=NULL;
		GH_DELETE_T( this, CAcceptorEx, MEMCATEGORY_GENERAL);
	}


	void	CAcceptorEx::SetClientLastPingTime()
	{
		m_uClientLastPingTime = GameHub::Timer::GetTicksMs();
	}

	void CAcceptorEx::SetDecodeKey(uint8 uKey)
	{
		m_uDecodeKey = uKey;
	}

	uint8 CAcceptorEx::GetDecodeKey()
	{
		return m_uDecodeKey;
	}

	void CAcceptorEx::SetEncodeKey(uint8 uKey)
	{
		m_uEncodeKey = uKey;
	}

	uint8 CAcceptorEx::GetEncodeKey()
	{
		return m_uEncodeKey;
	}

	void	CAcceptorEx::OnTick()
	{
		uint32 uCurTime = GameHub::Timer::GetTicksMs();
		uint32 uPastTime = uCurTime - m_uClientLastPingTime; 
		if(uPastTime > m_uCheckClientTick  << 1 ) // Àµ√˜π˝¡À∫‹æ√øÕªß∂À∂º√ª”–ping server±Ì√˜ «øÕªß∂À∂œœﬂ¡À
		{
			//PpeSqlLogEx(elc_Common, 0, "CAcceptorEx %u Ping Timeout : %u", this->GetId(), uPastTime);
			m_pFuncOnDisconnected(this); // Õ®÷™Õ‚≤ø¬ﬂº≠¥¶¿Ì
		}
	}


	void	CAcceptorEx::SetCallbackFunc(AcceptorExFuncOnDisconnected pOnDisconnected, 
																AcceptorExFuncOnSomeDataSend pOnSomeDataSend, 
																AcceptorExFuncOnSomeDataRecv pOnSomeDataRecv, 
																uint32 uCheckClientTick)
	{
		m_pFuncOnDisconnected = pOnDisconnected;
		m_pFuncOnSomeDataSend = pOnSomeDataSend;
		m_pFuncOnSomeDataRecv = pOnSomeDataRecv;

		
		if(uCheckClientTick)
		{
			m_uCheckClientTick = uCheckClientTick;
			GameHub::GetTickMgr()->RegisterTick(uCheckClientTick,this);
			SetClientLastPingTime();
		}
	}

	PipeResult		CAcceptorEx::Send(const void* pData, uint32 uSize)
	{
		return m_pAcceptor->Send(pData, uSize);
	}

	PipeResult		CAcceptorEx::Send(const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2)
	{
		return m_pAcceptor->Send(pData1,  uSize1, pData2,  uSize2);
	}

	PipeResult		CAcceptorEx::Send(const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2, const void* pData3, uint32 uSize3)
	{
		return m_pAcceptor->Send(pData1,  uSize1, pData2,  uSize2, pData3, uSize3);
	}

	void*	CAcceptorEx::GetRecvData()const
	{
		return m_pAcceptor->GetRecvData();
	}

	uint32	CAcceptorEx::GetRecvDataSize()
	{
		return m_pAcceptor->GetRecvDataSize();
	}

	uint32   CAcceptorEx::GetSendDataSize()
	{
		return m_pAcceptor->GetSendDataSize();
	}

	void		CAcceptorEx::PopRecvData(uint32 uSize)
	{
		m_pAcceptor->PopRecvData(uSize);
	}

	void		CAcceptorEx::SetMaxSendBufSize(uint32 uSize)
	{
		m_pAcceptor->SetMaxSendBufSize(uSize);
	}

	uint32	CAcceptorEx::GetMaxSendBufSize()
	{
		return m_pAcceptor->GetMaxSendBufSize();
	}

	void		CAcceptorEx::SetMaxRecvBufSize(uint32 uSize)
	{
		m_pAcceptor->SetMaxRecvBufSize(uSize);
	}

	uint32	CAcceptorEx::GetMaxRecvBufSize()
	{
		return m_pAcceptor->GetMaxRecvBufSize();
	}

	bool		CAcceptorEx::IsConnected()
	{
		return m_pAcceptor->IsConnected();
	}
	
	void CAcceptorEx::CbOnDisconnected(IAcceptor* pAcceptor)
	{
		CAcceptorEx* pAcceptorEx = static_cast<CAcceptorEx*>(pAcceptor->GetContext());
		pAcceptorEx->m_pFuncOnDisconnected(pAcceptorEx);
	}
	
	void CAcceptorEx::CbOnSomeDataSend(IAcceptor* pAcceptor)
	{
		CAcceptorEx* pAcceptorEx = static_cast<CAcceptorEx*>(pAcceptor->GetContext());
		pAcceptorEx->m_pFuncOnSomeDataSend(pAcceptorEx);
	}
	
	void CAcceptorEx::CbOnSomeDataRecv(IAcceptor* pAcceptor)
	{
		CAcceptorEx* pAcceptorEx = static_cast<CAcceptorEx*>(pAcceptor->GetContext());
		pAcceptorEx->m_pFuncOnSomeDataRecv(pAcceptorEx);
	}
}

