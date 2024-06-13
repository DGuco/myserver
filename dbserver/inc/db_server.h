/*****************************************************************
* FileName:db_server.h
* Summary :
* Date	  :2024-6-13
* Author  :DGuco(1139140909@qq.com)
******************************************************************/
#ifndef __DB_SERVER_H__
#define __DB_SERVER_H__

#include "singleton.h"
class CDBSerer : public CSingleton<CDBSerer>
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
	CDBSerer();
	//��������
	~CDBSerer();
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
	int SendMessageTo(CProxyMessage* pMsg);
	//��������
	int Event(CProxyMessage* pMsg);
	//����sql
	int ProcessExecuteSqlRequest(CProxyMessage* pMsg);
	//�ͷŽ��
	void ReleaseResult(QueryResult* res)
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
	int RegisterToProxyServer(CConnector* pConnector);
	//��proxy����������Ϣ
	int SendkeepAliveToProxy(CConnector* pConnector);
	//�ַ���Ϣ
	int DispatchOneCode(int nCodeLength, CByteBuff* pbyCode);
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
	static void lcb_OnConnected(IBufferEvent* pBufferEvent);
	//�Ͽ����ӻص�
	static void lcb_OnCnsDisconnected(IBufferEvent* pBufferEvent);
	//�ͻ����������ݻص�
	static void lcb_OnCnsSomeDataRecv(IBufferEvent* pBufferEvent);
	//����ʧ�ܻص�(����)
	static void lcb_OnConnectFailed(IBufferEvent* pBufferEvent);
	//�������ݻص�(����)
	static void lcb_OnCnsSomeDataSend(IBufferEvent* pBufferEvent);
	//������������ʱ�ص�
	static void lcb_OnPingServer(int fd, short event, CConnector* pConnector);
	static void lcb_OnSigPipe(uint, void*);

public:
	static CByteBuff m_acRecvBuff;
	static int m_iProxyId;
private:
	int m_iRunFlag;    // ���б�־
	CSafePtr<Database> m_pDatabase;
	//dbserver ==> gameserver
	CSafePtr<CShmMessQueue>	m_DB2SCodeQueue;
	//gameserver ==> dbserver
	CSafePtr<CShmMessQueue>	m_S2DBCodeQueue;
};
#endif //__DB_SERVER_H__
