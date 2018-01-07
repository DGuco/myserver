//
//  NetAddr.h
//  dpsg
//
//  Created by chopdown on 13-1-12.
//
//

#ifndef dpsg_NetAddr_h
#define dpsg_NetAddr_h

#include "net_Inc.h"

namespace Net {

class  NETWORK_API CNetAddr
{
public:
    CNetAddr(void)
    :m_uPort(0)
    {
        memset(m_szAddr, 0, sizeof(m_szAddr));
    }
    
    ~CNetAddr(void)
    {
        
    }
    
    CNetAddr(const char* szAddr, uint16 uPort)
    : m_uPort(uPort)
    {
        SetAddress(szAddr);
    }
    
    void SetAddress(const char* szAddr)
    {
#ifdef GH_OS_WIN32
        strncpy_s(m_szAddr, szAddr, 16);
#else
        strncpy(m_szAddr, szAddr, 16);
#endif
    }
    
    void Copy(const CNetAddr& addr)
    {
        SetAddress(addr.m_szAddr);
        SetPort(addr.m_uPort);
    }
    
    const char* GetAddress(void) const
    {
        return m_szAddr;
    }
    
    void SetPort(uint16 uPort)
    {
        m_uPort = uPort;
    }
    
    uint16 GetPort(void) const
    {
        return m_uPort;
    }
    
private:
    char		m_szAddr[16];
    uint16	m_uPort;
};

}


#endif
