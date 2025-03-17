#include "db_thread.h"

CDBThread::CDBThread(IDBThreadPool* pThreadPool)
{   
    m_pDatabase = new DatabaseMysql();
    m_pThreadPool = pThreadPool;
}

CDBThread::~CDBThread()
{
    m_pDatabase.Free();
}

bool CDBThread::PrepareToRun()
{
    m_pDatabase->ThreadStart();
    if(!m_pDatabase->Initialize("127.0.0.1;3306;root;000000;test",2))
    {
        return false;	
    }
	return true;
}

bool CDBThread::PrepareEnd()
{
    m_pDatabase->Close();
	m_pDatabase->ThreadEnd();
    return true;
}

void CDBThread::Run()
{
    while (!IsStoped())
	{
		m_pThreadPool->Save(m_pDatabase.DynamicCastTo<IDataBase>());
		//
		SLEEP(1);
	}
}