#pragma once

#include "network_interface.h"
#include "net_Inc.h"
#include "Socket.h"
#include "event_reactor.h"
#include "event.h"
#include "net_addr.h"

namespace Net
{
	class NETWORK_API CAcceptor : public IAcceptor
	{
		enum eAcceptorState
		{
			eAS_Disconnected=0,
			eAS_Connected,
		};

		enum eBufSize
		{
			eBS_MaxOutBufferSize = 1024 * 128, 
			eBS_MaxInBufferSize = 1024 * 16,
		};


	public:
		CAcceptor(SOCKET Socket, IEventReactor* pReactor);
		virtual ~CAcceptor(void);


		void	SetCallbackFunc(  AcceptorFuncOnDisconnected		pOnDisconnected,
											AcceptorFuncOnSomeDataSend	pOnSomeDataSend,
											AcceptorFuncOnSomeDataRecv	pOnSomeDataRecv);

		void		GetRemoteIpAddress(char* szBuf, uint32 uBufSize);
		PipeResult		Send(const void* pData, uint32 uSize);
		PipeResult		Send(const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2);
		PipeResult		Send(const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2, const void* pData3, uint32 uSize3);
		void*	GetContext(void)const;
		void		SetContext(void* pContext);
		void		ShutDown();
		void*	GetRecvData()const;				
		uint32	GetRecvDataSize();
		uint32   GetSendDataSize();
		void		PopRecvData(uint32 uSize);
		void		SetMaxSendBufSize(uint32 uSize);
		uint32	GetMaxSendBufSize();
		void		SetMaxRecvBufSize(uint32 uSize);
		uint32	GetMaxRecvBufSize();
		bool		IsConnected();
		void		Release();
	private:

		IEventReactor*	GetReactor();

		bool		RegisterToReactor();
		bool		UnRegisterFromReactor();	

		static void	lcb_OnPipeRead(struct bufferevent* bev, void* arg);
		static void	lcb_OnPipeWrite(bufferevent* bev, void* arg);
		static void	lcb_OnPipeError(bufferevent* bev, int16 nWhat, void* arg);

		eAcceptorState GetState();
		void SetState(eAcceptorState eState);


		void ProcessSocketError();

	private:
		void*												m_pContext;			// ��ҵ�������

		AcceptorFuncOnDisconnected			m_pFuncOnDisconnected;
		AcceptorFuncOnSomeDataSend		m_pFuncOnSomeDataSend;
		AcceptorFuncOnSomeDataRecv		m_pFuncOnSomeDataRecv;

		IEventReactor*				m_pReactor;
		CSocket						m_Socket;	
		eAcceptorState			m_eState;
		bufferevent*					m_pStBufEv;
		uint32							m_uMaxOutBufferSize;			//0��ʾ�����ƴ�С
		uint32							m_uMaxInBufferSize;			//0��ʾ�����ƴ�С
	};

}

