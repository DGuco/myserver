//
// dbctrl.h
// Created by DGuco on 17-7-13.
// Copyright ? 2018�� DGuco. All rights reserved.
//

#ifndef _DBCTRL_HPP_
#define _DBCTRL_HPP_

#include <memory>
#include "byte_buff.h"
#include "net_work.h"
#include "thread_pool.h"
#include "query_result.h"
#include "database.h"
#include "message_factory.h"
#include "config.h"

//#include "protoconfig.h"

using namespace std;

#ifdef _ASYNC_SQL_
#include "asqlexe.h"
#endif

#define MAXPROXYNUMBER                2                    // ���proxy server ��Ŀ

#define MAXHANDLENUMBER                2

#define SECSOFONEHOUR                3600

#define CHECKINTERVAL                10                    // ���ʱ��������λ����
#define PROXYKEEPALIVESECONDS        (3*CHECKINTERVAL)    // ��proxy��������ʱʱ��
#define MAXPROXYCODELEN                1024                // ��proxy���ʹ���������鳤��
#define STATISTICSLEN                1024
#define MAXNUMBERLEN                10
#define SAVEINTERVAL                300


class CSharedMem;

class CDBCtrl: public CSingleton<CDBCtrl>
{
public:
	// ���б�־
	enum ERunFlag
	{
		EFLG_CTRL_NO = 0,
		EFLG_CTRL_QUIT = 1,
		EFLG_CTRL_SHUTDOWN = 2,
		EFLG_CTRL_RELOAD = 3
	};
public:
	//���캯��
	CDBCtrl();
	//��������
	~CDBCtrl();
	//׼������
	int PrepareToRun();
	//����
	int Run();
	//�������б�־
	void SetRunFlag(int iFlag);
	//������б�־
	void ClearRunFlag(int iFlag);
	//���б�־�Ƿ�����
	bool IsRunFlagSet(int iFlag);
	//ת��Ц����
	int SendMessageTo(CProxyMessage *pMsg);
	//��������
	int Event(CProxyMessage *pMsg);
	//����sql
	int ProcessExecuteSqlRequest(CProxyMessage *pMsg);
	//�ͷŽ��
	void ReleaseResult(QueryResult *res)
	{
		if (res != NULL) {
			delete res;
			res = NULL;
		}
	}
private:
	//����proxy
	int ConnectToProxyServer();
	//��proxyע��
	int RegisterToProxyServer(CConnector *pConnector);
	//��proxy����������Ϣ
	int SendkeepAliveToProxy(CConnector *pConnector);
	//�ַ���Ϣ
	int DispatchOneCode(int nCodeLength, CByteBuff *pbyCode);
	//��ȡ�յ�������ʱ��
	time_t GetLastSendKeepAlive() const;
	//��ȡ�ϴη���������ʱ��
	time_t GetLastRecvKeepAlive() const;
	//�����ϴη���������ʱ��
	void SetLastSendKeepAlive(time_t tLastSendKeepAlive);
	//�����ϴ��յ�������ʱ��
	void SetLastRecvKeepAlive(time_t tLastRecvKeepAlive);
private:
	//���ӳɹ��ص�
	static void lcb_OnConnected(IBufferEvent *pBufferEvent);
	//�Ͽ����ӻص�
	static void lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent);
	//�ͻ����������ݻص�
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent);
	//����ʧ�ܻص�(����)
	static void lcb_OnConnectFailed(IBufferEvent *pBufferEvent);
	//�������ݻص�(����)
	static void lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent);
	//������������ʱ�ص�
	static void lcb_OnPingServer(int fd, short event, CConnector *pConnector);
    static void lcb_OnSigPipe(uint,void*);

public:
	static CByteBuff m_acRecvBuff;
	static int m_iProxyId;
private:
	int m_iRunFlag;    // ���б�־
	time_t m_tLastSendKeepAlive;        // �����proxy������Ϣʱ��
	time_t m_tLastRecvKeepAlive;        // ������proxy������Ϣʱ��
	shared_ptr<Database> m_pDatabase;
    shared_ptr<CMessageFactory> m_pMsgFactory;
//	CProxyHead m_stCurrentProxyHead;    //��ǰ���������Proxyͷ��
    shared_ptr<CNetWork>  m_pNetWork;
    shared_ptr<CServerConfig> m_pServerConfig;
};

#endif


