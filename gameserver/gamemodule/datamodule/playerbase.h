//
// Created by DGuco on 17-9-6.
// ��һ�����Ϣ
//

#ifndef SERVER_PLAYERBASE_H
#define SERVER_PLAYERBASE_H

#include <memory>
#include <ctime>
#include <cstring>
#include <iostream>
#include "playerdata.h"
#include "player.h"
#include "base.h"
#include "object.h"

// ���������Ϣ
struct STConnectInfo
{
    int						m_iSocket;									// socketid��FD
    time_t					m_tCreateTime;								// ����ʱ��
    time_t					m_tLastActiveTime;							// ����Ծʱ��
    time_t					m_tLoginTime;									// ��½ʱ��

    STConnectInfo()
    {
        Initialize();
    }

    int Initialize()
    {
        m_iSocket = 0;
        m_tCreateTime = 0;
        m_tLastActiveTime = 0;
        m_tLoginTime = 0;
        return 0;
    }

    void SetConnectInfo(int iSocketPara, time_t tCreateTimePara, time_t tLastActiveTimePara, time_t tLoginTimePara)
    {
        m_iSocket = iSocketPara;
        m_tCreateTime = tCreateTimePara;
        m_tLastActiveTime = tLastActiveTimePara;
        m_tLoginTime = tLoginTimePara;
    }

    void Clear()
    {
        m_iSocket = 0;
        m_tCreateTime = 0;
        m_tLastActiveTime = 0;
        m_tLoginTime = 0;
    }
};

class CPlayerBase : public CPlayerData
{
public:
    enum ETeamState
    {
        ETS_INVALID							= 0,	// ��Ч��״̬,Ҳ�ǳ�ʼ״̬
        ETS_STARTLOGIN						= 1,	// �մ���,��ʼ����������Ϣ,������ϢΪ��
        ETS_LOADACCOUNT				        = 2,	// �յ�½,��ȡ�ʺ���Ϣ
        ETS_LOADDATA						= 3,	// ��ȡս������
        ETS_INGAMECONNECT			        = 4,	// ����Ϸ��,����������
        ETS_INGAMEDISCONNECT		        = 5,	// ����Ϸ��,������
        ETS_EXITSAVE						= 6,	// �뿪�洢����
    };

    enum ESLF_FLAGS
    {
        ELSF_LOADALL	= 0x0,  // todo fix it
        ELSF_SAVEALL	= 0x1,
    };

    enum emFirstLoginFlag
    {
        emLoginFlag_First				= 0,		// ��һ�ε�¼
        emLoginFlag_notFirst		= 1,		// ���ǵ�һ�ε�¼
        emLoginFlag_haveobj			= 2,		// ��Ҵ���ʵ��
    };

    // �����������
    // < 0 tcp�����������⵽���ӶϿ�
    // = 0 ��������
    // > 0 ��������������
    enum ELeaveType
    {
        LEAVE_CLIENTCLOSE		= -1,		// TCP��⵽�ͻ��˶���,����Ҫ�ظ�
        LEAVE_TIMEOUT				= -2,		// TCP��⵽�ͻ��˳�ʱ,����Ҫ�ظ�
        LEAVE_ERRPACKAGE			= -3,		// TCP��⵽����Ϣ�Ƿ�,����Ҫ�ظ�
        LEAVE_BUFFOVER				= -4,		// TCP����������,����Ҫ�ظ�
        LEAVE_PIPEERR					= -5,		// TCP���ݰ�����ܵ�ʧ��,����Ҫ�ظ�
        LEAVE_SYSTEM					= -6,		// TCPϵͳ������ʱδ��,����Ҫ�ظ�
        LEAVE_PUSHAGAINST		= -7,		// ������ȥ,����Ҫ�ظ�
        LEAVE_NOCONTRAL			= -8,		// ��ҳ�ʱ��δ����,����Ҫ�ظ�
        LEAVE_NORMAL 				= 0,		// �����������,��Ҫ�ظ�
        LEAVE_LOGINFAILED		= 1,		// ��ҵ�½ʧ��,��Ҫ�ظ�
    };

public:
    CPlayerBase(CPlayer* pPlayer);
    virtual ~CPlayerBase();
    virtual int Initialize();
    // ʵ��ID
    OBJ_ID GetEntityID() {return GetPlayerId();}

public:
    // �ϴε�¼ʱ��
    void SetLastLoginTime(int v) {m_iLastLoginTime = v;}
    int GetLastLoginTime() {return m_iLastLoginTime;}
    // ����ʱ��
    void SetLeaveTime(int v) {m_iLeaveTime = v;}
    int GetLeaveTime() {return m_iLeaveTime;}
    // ����ʱ��
    void SetCreateTime(time_t v) {m_tCreateTime = v;}
    time_t GetCreateTime() {return m_tCreateTime;}

    int GetSocket() {return m_SocketInfo.m_iSocket;}
    time_t GetSocketCreateTime() { return  m_SocketInfo.m_tCreateTime;}
    time_t GetLastActiveTime() {return m_SocketInfo.m_tLastActiveTime;}
    time_t GetLoginTime() {return m_SocketInfo.m_tLoginTime;}
    // �ʺ�
    void SetAccount(const char* p)
    { }
    char* GetAccount() {return &m_acAccount[0];}
    // ��ȡ������Ϣ
    STConnectInfo* GetSocketInfoPtr() {return &m_SocketInfo;}

private:
    // �ϴε�¼ʱ��
    int m_iLastLoginTime;
    // ����ʱ��
    int m_iLeaveTime;
    // ����ʱ��
    time_t m_tCreateTime;
    // �ʺ�״̬ʱ��( ��ֹ��½ ��ֹ˵�� ��ֹ...)
    time_t m_tLoginLimitTime;
    // �ʺ�
    char			m_acAccount[UID_LENGTH];
    //������Ϣ
    STConnectInfo	m_SocketInfo;
};


#endif //SERVER_PLAYERBASE_H
