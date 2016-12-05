#ifndef _PROTO_CONFIG_H_
#define _PROTO_CONFIG_H_


#include "servertool.h"
#include "../../message/configure.pb.h"


class CConfigMgr : public CSingleton<CConfigMgr>
{
public:
	CConfigMgr()	{}
	~CConfigMgr() {}

	CConfigure* GetConfig() {return &m_Config;}
	const ::PBHallConfig& GetHallConfig() {return m_Config.hallconfig();}
	const ::PBDBConfig& GetDBConfig() {return m_Config.dbconfig();}
	const ::PBProxyConfig& GetProxyConfig() {return m_Config.proxyconfig();}
	const ::PBGateConfig& GetGateConfig() {return m_Config.gateconfig();}
	const ::PBTcpConfig& GetTcpConfig() {return m_Config.tcpconfig();}
	const ::PBClientConfig& GetClientConfig() {return m_Config.clientconfig();}
	const ::PBRoomConfig& GetRoomConfig() {return m_Config.roomconfig();}

	int LoadConfig(std::string pPathFileName);

protected:
	CConfigure m_Config;
};


#endif //_PROTO_CONFIG_H_



