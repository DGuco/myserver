#pragma once
#include "network_interface.h"
#include "net_work.h"
//#include "TNetWork.h"
#include "TickMgr.h"
#include "Types.h"


namespace Net
{
	INTER_FACE IAcceptor;


	class NETWORK_API CAcceptorEx : public GameHub::CTick
	{
	public:
		CAcceptorEx(uint32 uId, IAcceptor* pAcceptor);
		virtual ~CAcceptorEx(void);

		uint32 GetId() const;


		void	SetCallbackFunc(  AcceptorExFuncOnDisconnected		pOnDisconnected,
											AcceptorExFuncOnSomeDataSend	pOnSomeDataSend,
											AcceptorExFuncOnSomeDataRecv	pOnSomeDataRecv,
											uint32 uCheckClientTick);


		void		GetRemoteIpAddress(char* szBuf, uint32 uBufSize);

		PipeResult		Send(const void* pData, uint32 uSize);
		PipeResult		Send(const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2);
		PipeResult		Send(const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2, const void* pData3, uint32 uSize3);
		void*	GetRecvData()const;
		uint32	GetRecvDataSize();
		uint32   GetSendDataSize();
		void		PopRecvData(uint32 uSize);
		void		SetMaxSendBufSize(uint32 uSize);
		uint32	GetMaxSendBufSize();
		void		SetMaxRecvBufSize(uint32 uSize);
		uint32	GetMaxRecvBufSize();
		bool		IsConnected();

		void		SetClientLastPingTime();
		void		OnTick();
		const char* GetTickName() { return "CAcceptorEx Tick"; };
		void     SetDecodeKey(uint8 uKey);
		uint8 GetDecodeKey();

		void SetEncodeKey(uint8 uKey);
		uint8 GetEncodeKey();

	protected:
		void ShutDown();
		void Release();

	private:
		static void CbOnDisconnected(IAcceptor* pAcceptor);
		static void CbOnSomeDataSend(IAcceptor* pAcceptor);
		static void CbOnSomeDataRecv(IAcceptor* pAcceptor);
		



	private:
		uint32				m_uId;
		IAcceptor*		m_pAcceptor;
		uint32				m_uClientLastPingTime;
		uint32				m_uCheckClientTick;
		uint8				m_uDecodeKey;
		uint8				m_uEncodeKey;

		AcceptorExFuncOnDisconnected		m_pFuncOnDisconnected;
		AcceptorExFuncOnSomeDataSend	m_pFuncOnSomeDataSend;
		AcceptorExFuncOnSomeDataRecv	m_pFuncOnSomeDataRecv;
	};
}
