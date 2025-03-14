//
// database.h
// Created by DGuco on 17-7-13.
// Copyright ? 2018Äê DGuco. All rights reserved.
//

#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H

#include "log.h"
#include <string>
#include <string.h>

#define MAX_QUERY_LEN   1024

class QueryResult;

class IDataBase
{
public:
    //
    IDataBase();
    //
    virtual ~IDataBase();
    //
    virtual QueryResult *Query(const char *sql, unsigned long len) = 0;
    //
    virtual QueryResult *QueryForprocedure(const char *sql, unsigned long len, int number)=0;
    //
    QueryResult *PQuery(const char *format, ...);
    //
    virtual bool Execute(const char *sql) = 0;
    //
    bool PExecute(const char *format, ...);
    //
    virtual bool DirectExecute(const char *sql) = 0;
    //
    virtual void ThreadStart() = 0;
	//
    virtual void ThreadEnd() = 0;
    //
    virtual bool RealDirectExecute(const char *sql, unsigned long len) = 0;
    //
    bool DirectPExecute(const char *format, ...);
    // Writes SQL commands to a LOG file (see mangosd.conf "LogSQL")
    bool PExecuteLog(const char *format, ...);
    virtual bool BeginTransaction()
    {
        return true;
    }
    virtual bool CommitTransaction()
    {
        return true;
    }
    virtual bool RollbackTransaction()
    {
        return false;
    }

    virtual operator bool() const = 0;

    virtual unsigned long escape_string(char *to, const char *from, unsigned long length)
    {
        strncpy(to, from, length);
        return length;
    }
    void escape_string(std::string &str);    
private:
    bool m_logSQL;
    std::string m_logsDir;

protected:
    std::string m_hostInfoString;
    std::string m_logsName;
};

#endif //SERVER_DATABASE_H
