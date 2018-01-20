//
//  socket.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//


#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <base.h>
#include "net_inc.h"
#include "event.h"

class CNetAddr;

class CSocket {
 public:
  CSocket(SOCKET Socket);
  CSocket(int32 nType = SOCK_STREAM, int32 nProtocolFamily = AF_INET, int32 nProtocol = 0);
  virtual ~CSocket();

  virtual bool Open();

  virtual void Close();

  void Shutdown();
  void ShutdownRead();
  void ShutdownWrite();

  void SetNonblocking();

  uint32 Bind(const CNetAddr &Address);

  bool GetLocalAddress(CNetAddr &Address) const;
  bool GetRemoteAddress(CNetAddr &Address) const;

  SOCKET GetSystemSocket() const { return m_Socket; }

  void SetSystemSocket(SOCKET Socket) { m_Socket = Socket; }

  int GetSocketError() const;
  SOCKET GetSocket() const;

  static SOCKET CreateSocket(int32 Type = SOCK_STREAM, int32 ProtocolFamily = AF_INET, int32 Protocol = 0);
  static SOCKET CreateBindedSocket(const CNetAddr &address);
  static void MakeSocketNonblocking(SOCKET Socket);
  static void Address2sockaddr_in(sockaddr_in &saiAddress, const CNetAddr &Address);

 protected:
  SOCKET m_Socket;

 private:
  int32 m_nType;
  int32 m_nProtocolFamily;
  int32 m_nProtocol;
};

#endif
