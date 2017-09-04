//
// Created by DGuco on 17-7-13.
//

#ifndef SERVER_DBHANDLE_H
#define SERVER_DBHANDLE_H

#include "../../framework/base/servertool.h"
#include "../../framework/thread/mythread.h"
#include "../../framework/net/tcp_conn.h"
#include "../../framework/mem/shm.h"
#include "../../framework/mem/codequeue.h"
#include "../../framework/message/proxymessage.pb.h"
#include "../../framework/message/message.pb.h"
#include "../inc/basedb.h"
#include "../inc/queryresultmysql.h"
#include "../inc/database.h"
#include "../inc/messagefactory.h"


#define HASHVALUE					512
#define VALUENEEDEDPERLEVEL			100
#define SAVEBRAINRECORDINTERVAL		1382400  //16天的秒数
#define SECONDS_OF_YEAR				365*24*60*60
#define INPUTQUEUELENGTH			0x1000000

#define MAXERRORMSGLEN				512                  //错误内存块最大长度
#define MAXMSGSTATNUM				203

class CMessageFactory;

enum enDBOperMode
{
    DBO_Cache = 1,			//cache 操作方式
    DBO_DirectUpdate = 2,	//直接更新方式
};


struct STMsgProcessStatInfo
{
    int m_iMsgID;
    int m_iMsgTotal;
    timeval m_tvMsgProcTotal;
    timeval m_tvMaxMsgProc;
};

class CDBHandle : public CMyThread
{
public:
    CDBHandle();
    ~CDBHandle();

    enum _enHandleParas
    {
        //HANDLECOUNT = 8,
        MULTIFACTOR = 1,
        RPTCHECKGAP = 3600,
        ROUTETABSIZE = 1024
    };

    virtual bool IsToBeBlocked();
    virtual int PrepareToRun();
    virtual int Run();
    virtual int ProcessThreadEnd();

    int Initialize( int iHandleID, CTCPConn<RECVBUFLENGTH, POSTBUFLENGTH>* pGSvrdConns);

    int PostOneCode( int nCodeLength, BYTE* pCode);

    int InitLogFile( const char* vLogName, const char* vLogDir, LogLevel vPriority  = LEVEL_NOTSET , unsigned int vMaxFileSize  = 10*1024*1024 , unsigned int vMaxBackupIndex  = 1 , bool vAppend  = true  );
    static CSharedMem *ms_pCurrentShm;
    static int m_iDBSvrID;
    static short m_sDBOperMode;
    static std::mutex m_sMutex;
    int GetHandleID(void){ return m_iHandleID; }

    int ConnectToLocalDB();
private:

    int GetOneCode(int& nCodeLength, BYTE* pCode);

    int SendMessageTo(CMessage *pMsg);
    int Event(CMessage *pMsg);

    int ProcessExecuteSqlRequest( CMessage* pMsg );

    void ReleaseResult(QueryResult *res)
    {
        if( res != NULL )
        {
            delete res;
            res = NULL;
        }
    }


private:

    int m_iHandleID;

    Database* m_pDatabase;

    CProxyHead m_stCurrentProxyHead;							//当前处理请求的Proxy头部

    CTCPConn<RECVBUFLENGTH, POSTBUFLENGTH> m_stProxySvrdConns;	//指向所有的proxy server 链接

    time_t m_tLastCheckTime;

    CCodeQueue* m_pInputQueue;									//输入消息队列

    CMessageFactory* mMsgFactory;
};


#endif //SERVER_DBHANDLE_H
