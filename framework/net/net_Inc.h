//
//  NetInc.h
//  dpsg
//
//  Created by chopdown on 13-1-12.
//
//

#ifndef dpsg_NetInc_h
#define dpsg_NetInc_h

#include "Common/Types.h"
#include "Common/Log.h"
#include "Types.h"
#include "Log.h"

#ifdef GH_OS_WIN32
    #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

    #ifdef NETWORK_EXPORTS
        #define NETWORK_API __declspec(dllexport)
    #else
        #define NETWORK_API __declspec(dllimport)
    #endif

    #ifdef NETWORK_STATIC
        #undef NETWORK_API
        #define NETWORK_API
    #endif

#else
    #define NETWORK_API
#endif


#ifdef GH_OS_WIN32
#define PpeGetLastError		WSAGetLastError
#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
//#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
//#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE
#define SHUT_RDWR               SD_BOTH
#define SHUT_RD					SD_RECEIVE
#define SHUT_WR                 SD_SEND

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>


#define PpeGetLastError() errno
#define closesocket			close
#define ioctlsocket			ioctl
#define SOCKET			int
typedef struct linger 		LINGER;
#define SOCKET_ERROR		-1
#define INVALID_SOCKET		-1
#define SD_SEND			SHUT_WR

#endif

#define INTER_FACE struct


namespace Net{
    using namespace GameHub::BaseTypes;
    
    enum EPipeDisconnectReason
    {
        ePDR_REMOTE,
        ePDR_ERROR
    };
    
    enum EPipeConnFailedReason
    {
        ePCFR_UNREACH,
        ePCFR_INVALIDADDR,
        ePCFR_REFUSED,
        ePCFR_RESET,
        ePCFR_LOCALADDRINUSE,
        ePCFR_NOBUFFER,
        ePCFR_TIMEDOUT
    };

}


#endif
