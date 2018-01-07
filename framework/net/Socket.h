//
//  Socket.h
//  dpsg
//
//  Created by chopdown on 13-1-12.
//
//

#ifndef dpsg_Socket_h
#define dpsg_Socket_h

#include "net_Inc.h"


#include "event.h"

namespace Net
{
    class CNetAddr;
    
	class NETWORK_API CSocket
	{
	public:
		CSocket(SOCKET Socket);
		CSocket(int32 nType=SOCK_STREAM, int32 nProtocolFamily=AF_INET, int32 nProtocol=0);
		virtual ~CSocket();

		virtual bool	Open ();

		virtual void	Close ();

		void	Shutdown ();
		void	ShutdownRead ();
		void	ShutdownWrite ();

		void	SetNonblocking();

		uint32	Bind(const CNetAddr& Address);
        
		bool	GetLocalAddress(CNetAddr& Address) const;
		bool	GetRemoteAddress(CNetAddr& Address) const;
        
        
		SOCKET	GetSystemSocket () const			{return m_Socket;}
        
		void	SetSystemSocket (SOCKET Socket)	{m_Socket=Socket;}
        
		
		int		GetSocketError()const;
        

		static SOCKET CreateSocket(int32 Type=SOCK_STREAM, int32 ProtocolFamily=AF_INET, int32 Protocol=0);
		static SOCKET CreateBindedSocket(const CNetAddr& address);
		static void	MakeSocketNonblocking(SOCKET Socket);
		static void Address2sockaddr_in(sockaddr_in& saiAddress,const CNetAddr& Address);
        
	protected:
		SOCKET m_Socket;
        
	private:
		int32 m_nType;
		int32 m_nProtocolFamily;
		int32 m_nProtocol;
	};
}


#endif
