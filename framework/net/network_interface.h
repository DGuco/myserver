//
//  INetWork.h
//  dpsg
//
//  Created by chopdown on 13-1-12.
//
//

#ifndef dpsg_INetWork_h
#define dpsg_INetWork_h
#include "net_Inc.h"

namespace Net{
	INTER_FACE	IEventReactor;
	INTER_FACE IReactorHandler;
	
	INTER_FACE	IConnectorHandler;
	INTER_FACE	IConnector;
    
    
    INTER_FACE	IListenerHandler;
	INTER_FACE	IListener;
    
    INTER_FACE	IAcceptorHandler;
	INTER_FACE	IAcceptor;
    
    class CNetAddr;
    
	enum PipeResult
	{
		ePR_OK = 0,
		ePR_Disconnected,
		ePR_OutPipeBuf,
	};
	
	/** Interface for IEventReactor
     */
	INTER_FACE IEventReactor
	{
		virtual bool Register(IReactorHandler* pHandler)=0;
		virtual bool UnRegister(IReactorHandler* pHandler)=0;
		virtual void*	GetEventBase()	=0;
		virtual void Init()=0;
		virtual void DispatchEvents()=0;
		virtual void Release()=0;
	};
    
	INTER_FACE IReactorHandler
	{
		virtual bool RegisterToReactor()=0;
		virtual bool UnRegisterFromReactor()=0;
		virtual IEventReactor*	GetReactor()=0;
	};
    
    
    /** Interface for IListener*/
	typedef void(*ListenerFuncOnAccept)(IEventReactor* pReactor, SOCKET Socket);
    
    
	INTER_FACE IListener : public IReactorHandler
	{
		virtual bool Listen(CNetAddr& addr, ListenerFuncOnAccept pFunc)=0;
		virtual bool IsListened(void)=0;
		virtual void ShutDown()=0;
		virtual void Release()=0;
	};
    
    
	/** Interface for IConnector
     */
	typedef void(*ConnectorFuncOnDisconnected)(IConnector* pConnector);
	typedef void(*ConnectorFuncOnConnectFailed)(IConnector* pConnector);
	typedef void(*ConnectorFuncOnConnectted)(IConnector* pConnector);
	typedef void(*ConnectorFuncOnSomeDataSend)(IConnector* pConnector);
	typedef void(*ConnectorFuncOnSomeDataRecv)(IConnector* pConnector);
    
	INTER_FACE IConnector : public IReactorHandler
	{
		virtual void		GetRemoteIpAddress(char* szBuf, uint32 uBufSize)=0;
		virtual bool		Connect(const CNetAddr& addr, const timeval * time = NULL)=0;
		virtual void		SetCallbackFunc( ConnectorFuncOnDisconnected		pOnDisconnected,
                                            ConnectorFuncOnConnectFailed		pOnConnectFailed,
                                            ConnectorFuncOnConnectted			pOnConnectted,
                                            ConnectorFuncOnSomeDataSend		pOnSomeDataSend,
                                            ConnectorFuncOnSomeDataRecv		pOnSomeDataRecv)=0;
		virtual void*		GetContext(void) const=0;
		virtual void		SetContext(void* pContext)=0;
		virtual PipeResult	Send(const void* pData, uint32 uSize)=0;
		virtual void*		GetRecvData()const=0;
		virtual uint32	GetRecvDataSize()=0;
		virtual uint32	GetSendDataSize()=0;
		virtual void		PopRecvData(uint32 uSize)=0;
		virtual void		SetMaxSendBufSize(uint32 uSize)=0;
		virtual uint32	GetMaxSendBufSize()=0;
		virtual void		SetMaxRecvBufSize(uint32 uSize)=0;
		virtual uint32	GetMaxRecvBufSize()=0;
		virtual bool		IsConnected()=0;
		virtual void		ShutDown()=0;
		virtual void		Release()=0;
	};
    
    /** Interface for IAcceptor
     */
	typedef void(*AcceptorFuncOnDisconnected)(IAcceptor* pAcceptor);
	typedef void(*AcceptorFuncOnSomeDataSend)(IAcceptor* pAcceptor);
	typedef void(*AcceptorFuncOnSomeDataRecv)(IAcceptor* pAcceptor);
    
    INTER_FACE IAcceptor: public IReactorHandler
	{
		virtual void		SetCallbackFunc( AcceptorFuncOnDisconnected			pOnDisconnected,
                                            AcceptorFuncOnSomeDataSend		pOnSomeDataSend,
                                            AcceptorFuncOnSomeDataRecv		pOnSomeDataRecv)=0;
        
		virtual void		GetRemoteIpAddress(char* szBuf, uint32 uBufSize)=0;
		virtual PipeResult		Send(const void* pData, uint32 uSize)=0;
		virtual PipeResult		Send(const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2)=0;
		virtual PipeResult		Send(const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2, const void* pData3, uint32 uSize3)=0;
		virtual void*		GetContext(void)const=0;
		virtual void		SetContext(void* pContext)=0;
		virtual void*		GetRecvData()const=0;
		virtual uint32	GetSendDataSize()=0;
		virtual uint32	GetRecvDataSize()=0;
		virtual void		PopRecvData(uint32 uSize)=0;
		virtual void		SetMaxSendBufSize(uint32 uSize)=0;
		virtual uint32	GetMaxSendBufSize()=0;
		virtual void		SetMaxRecvBufSize(uint32 uSize)=0;
		virtual uint32	GetMaxRecvBufSize()=0;
		virtual bool		IsConnected()=0;
		virtual void		ShutDown()=0;
		virtual void		Release()=0;
	};
    
	typedef void(*OnSignal)(uint32 uSignal, void* pContext);
    
	INTER_FACE ISystemSignal : public IReactorHandler
	{
		virtual void SetCallBackSignal(uint32 uSignal, OnSignal pFunc, void* pContext, bool bLoop = false)=0;
		virtual void Release()=0;
	};
    
	NETWORK_API IEventReactor* GetEventReactor();
	NETWORK_API IConnector*		GetConnector(IEventReactor* pReactor);
	NETWORK_API ISystemSignal*	GetSystemSignal(IEventReactor* pReactor);
    NETWORK_API IAcceptor*		GetAcceptor(SOCKET Socket, IEventReactor* pReactor);
    NETWORK_API IListener*			GetListener(IEventReactor* pReactor);
    
}



#endif
