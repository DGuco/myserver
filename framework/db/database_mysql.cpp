//
// Created by DGuco on 17-7-13.
//

#include <string>
#include "database_mysql.h"
#include "query_result_mysql.h"

using namespace std;

void DatabaseMysql::ThreadStart()
{
    mysql_thread_init();
}

void DatabaseMysql::ThreadEnd()
{
    mysql_thread_end();
}

size_t DatabaseMysql::db_count = 0;

DatabaseMysql::DatabaseMysql() : IDataBase(), m_pMysql(0)
{
    // before first connection
    if( db_count++ == 0 )
    {
        // Mysql Library Init
        mysql_library_init(-1, NULL, NULL);

         //如果是每个线程独享一个mysql链接，那么就不需要mysql的线程安全性
         if (!mysql_thread_safe())
         {
            DISK_LOG( DB_ERROR, "FATAL ERROR: Used MySQL library isn't thread-safe.");
            exit(1);
         }
    }

    m_rwtimeout = 30;
    m_sleeptime = 10;
    m_loop = 10;
}

DatabaseMysql::~DatabaseMysql()
{
    Close();
    //Free Mysql library pointers for last ~DB
    if(--db_count == 0)
        mysql_library_end();

    // m_connflag = 0;
    m_rwtimeout = 0;
    m_sleeptime = 0;
    m_loop = 0;
}

Tokens DatabaseMysql::StrSplit(const std::string& src, const std::string& sep)
{
	Tokens r;
	std::string s;
	for (std::string::const_iterator i = src.begin(); i != src.end(); i++)
	{
		if (sep.find(*i) != std::string::npos)
		{
			if (s.length()) r.push_back(s);
			s = "";
		}
		else
		{
			s += *i;
		}
	}
	if (s.length()) r.push_back(s);
	return r;
}

bool DatabaseMysql::Initialize(const char *infoString, int rw_timeout, int sleep_time, int loop)
{
    m_hostInfoString = std::string( infoString );

    m_rwtimeout = rw_timeout;
    m_sleeptime = sleep_time;
    m_loop = loop;

    Tokens tokens = StrSplit(m_hostInfoString, ";");

    Tokens::iterator iter;

    iter = tokens.begin();

    if(iter != tokens.end())
        m_host = *iter++;
    if(iter != tokens.end())
        m_port_or_socket = *iter++;
    if(iter != tokens.end())
        m_user = *iter++;
    if(iter != tokens.end())
        m_password = *iter++;
    if(iter != tokens.end())
        m_database = *iter++;

    if (Connect() != true)
    {
        return Reconnect();
    }
    else
    {
        return true;
    }
}

bool DatabaseMysql::Reconnect()
{
    for (int i = 0; i < m_loop; i++)
    {
        SLEEP(m_sleeptime);

        if (Connect() == true)
        {
            return true;
        }
    }

    return false;
}

bool DatabaseMysql::Connect()
{
    MYSQL *mysqlInit = mysql_init(NULL);
    if (!mysqlInit)
    {
        DISK_LOG(DB_ERROR,"Could not initialize Mysql connection" );
        return false;
    }

    int port;
    char const* unix_socket;

    mysql_options(mysqlInit,MYSQL_OPT_READ_TIMEOUT, (const void *)&m_rwtimeout);
    mysql_options(mysqlInit,MYSQL_OPT_WRITE_TIMEOUT,(const void *)&m_rwtimeout);
    //mysql_options(mysqlInit,MYSQL_SET_CHARSET_NAME,"gbk");
#ifdef WIN32
    if(m_host==".")                                           // named pipe use option (Windows)
    {
        unsigned int opt = MYSQL_PROTOCOL_PIPE;
        mysql_options(mysqlInit,MYSQL_OPT_PROTOCOL,(char const*)&opt);
        port = 0;
        unix_socket = 0;
    }
    else                                                    // generic case
    {
        port = atoi(m_port_or_socket.c_str());
        unix_socket = 0;
    }
#else
    if(m_host==".")                                           // socket use option (Unix/Linux)
    {
        unsigned int opt = MYSQL_PROTOCOL_SOCKET;
        mysql_options(mysqlInit,MYSQL_OPT_PROTOCOL,(char const*)&opt);
        m_host = "localhost";
        port = 0;
        unix_socket = m_port_or_socket.c_str();
    }
    else                                                    // generic case
    {
        port = atoi(m_port_or_socket.c_str());
        unix_socket = 0;
    }
#endif

    //mMysql = mysql_real_connect(mysqlInit, host.c_str(), user.c_str(),
    //   password.c_str(), database.c_str(), port, unix_socket, CLIENT_MULTI_STATEMENTS);

    DISK_LOG( DEBUG_DISK, "Connected to MySQL database : rwtimeout = {} ; sleeptime = {} ; loop = {}", m_rwtimeout, m_sleeptime, m_loop);

    m_pMysql = mysql_real_connect(mysqlInit, m_host.c_str(), m_user.c_str(),
                                m_password.c_str(), m_database.c_str(), port, unix_socket, 0);
    CACHE_LOG(DB_CACHE, "host : {} ; user = {} ; password = {} ; database = {} ; port = {}", m_host.c_str(), m_user.c_str(), m_password.c_str(), m_database.c_str(), port );

    mysql_options(mysqlInit, MYSQL_OPT_RECONNECT, (char*)"1");

    if (m_pMysql)
    {
        CACHE_LOG(DB_CACHE, "Connected to MySQL database at {}", m_host.c_str());
        CACHE_LOG(DB_CACHE, "MySQL client library: {}", mysql_get_client_info());
        CACHE_LOG(DB_CACHE, "MySQL server ver: {} ", mysql_get_server_info( m_pMysql));

        /*----------SET AUTOCOMMIT ON---------*/
        // It seems mysql 5.0.x have enabled this feature
        // by default. In crash case you can lose data!!!
        // So better to turn this off
        // ---
        // This is wrong since mangos use transactions,
        // autocommit is turned of during it.
        // Setting it to on makes atomic updates work
        if (!mysql_autocommit(m_pMysql, 1))
            CACHE_LOG(DB_CACHE, "AUTOCOMMIT SUCCESSFULLY SET TO 1");
        else
            CACHE_LOG(DB_CACHE, "AUTOCOMMIT NOT SET TO 1");
        /*-------------------------------------*/

        // set connection properties to UTF8 to properly handle locales for different
        // server configs - core sends data in UTF8, so MySQL must expect UTF8 too
        DirectExecute("SET NAMES utf8;");
        DirectExecute("SET character_set_database = utf8;");
        DirectExecute("SET character_set_server = utf8;");
        DirectExecute("SET collation_connection = utf8_general_ci;");
        DirectExecute("SET collation_database = utf8_general_ci;");
        DirectExecute("SET collation_server = utf8_general_ci;");
        //DirectExecute("SET CHARACTER SET utf8;");

        //PExecute("SET NAMES `gbk`");
        //PExecute("SET CHARACTER SET `gbk`");

        // m_connflag = 1;
        return true;
    }
    else
    {
        // int nErrorNo = mysql_errno( mysqlInit );
        DISK_LOG( DB_ERROR, "Could not connect to MySQL database at {}: {}\n", m_host.c_str(),mysql_error(mysqlInit));
        mysql_close(mysqlInit);
        // m_connflag = 0;

        // if (nErrorNo == CR_CONN_HOST_ERROR)
        //     return nErrorNo;

        return false;
    }
}

bool DatabaseMysql::IsConnected() 
{
    if (m_pMysql == NULL) 
    {
        return false;
    }

    if (mysql_ping(m_pMysql) == 0) {
        return true;
    } else 
    {
        // 如果连接已经断开，尝试重新连接
        if (Reconnect() == true) {
            return true;
        } else 
        {
            return false;
        }
    }
}

void DatabaseMysql::Close()
{
    if (m_pMysql)
    {
        mysql_close(m_pMysql);
        m_pMysql = NULL;
    }
}

QueryResult* DatabaseMysql::Query(const char *sql, unsigned long len)
{
    if (!m_pMysql)
    {
        if (IsConnected() == false)
        {
            return NULL;
        }
    }

    MYSQL_RES *result = 0;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;

    {
        //--------------------------------------------------
        // int ret = mysql_real_query(mMysql, sql, len);
        //--------------------------------------------------
        int ret = mysql_query(m_pMysql, sql);
        if( ret )
        {
            int nErrorNo = mysql_errno( m_pMysql );
            // if mysql has gone, maybe longtime no request or mysql restarted
            if( nErrorNo == CR_SERVER_GONE_ERROR || nErrorNo == CR_SERVER_LOST || nErrorNo == CR_UNKNOWN_ERROR/* || nErrorNo == CR_PROXY_ERROR*/)
            {
                Close();

                // reconnect mysql
                if ( Reconnect() == true )
                {
                    CACHE_LOG( DB_CACHE, "reinit mysql success on host [{}]", m_hostInfoString.c_str() );

                    // re querey  sql statment
                    //--------------------------------------------------
                    // ret =  mysql_real_query(mMysql, sql, len);
                    //--------------------------------------------------
                    ret = mysql_query(m_pMysql, sql);
                }
            }

            if( ret )
            {
                DISK_LOG( DB_ERROR, "SQL: {}", sql );
                DISK_LOG(DB_ERROR, "query ERROR({}): {}", nErrorNo, mysql_error(m_pMysql) );
                return NULL;
            }

        }
        else
        {
            // 加入统计该sql语句执行多久
        }

        result = mysql_store_result(m_pMysql);

        rowCount = mysql_affected_rows(m_pMysql);
        fieldCount = mysql_field_count(m_pMysql);
        // end guarded block
    }

    if (!result )
        return NULL;

    //if (!rowCount)
    //{
    //    mysql_free_result(result);
    //    return NULL;
    //}

    QueryResultMysql *queryResult = new QueryResultMysql(result, rowCount, fieldCount);
    if( queryResult == NULL )
    {
        DISK_LOG(DB_ERROR, "while create qureyresult, run out of memory" );
        return NULL;
    }
    queryResult->NextRow();

    return queryResult;
}

QueryResult* DatabaseMysql::QueryForprocedure(const char *sql, unsigned long len, int number)
{
    if (!m_pMysql)
    {
        if (IsConnected() == false)
        {
            return NULL;
        }
    }

    MYSQL_RES *result = 0;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;

    {
        int ret = mysql_query(m_pMysql, sql);
        if( ret )
        {
            int nErrorNo = mysql_errno( m_pMysql );
            // if mysql has gone, maybe longtime no request or mysql restarted
            if( nErrorNo == CR_SERVER_GONE_ERROR || nErrorNo == CR_SERVER_LOST || nErrorNo == CR_UNKNOWN_ERROR /*|| nErrorNo == CR_PROXY_ERROR*/ )
            {
                Close();

                // reconnect mysql
                if ( Reconnect() == true )
                {
                    CACHE_LOG( DB_CACHE, "reinit mysql success on host [{}]", m_hostInfoString.c_str() );
                    ret = mysql_query(m_pMysql, sql);

                    if ( !ret )
                    {
                        string strProcSql = "SELECT ";
                        char   acNumber[32] = {0};
                        for ( int i = 1; i < number+1; i++ )
                        {
                            memset( acNumber, 0, sizeof(acNumber));
                            snprintf(acNumber, sizeof(acNumber), "{}", i);
                            string strNumber = acNumber;
                            string strPara = "@out_para" + strNumber;
                            strProcSql += strPara;
                            if ( i < number )
                            {
                                strProcSql += ",";
                            }
                        }

                        mysql_query(m_pMysql, strProcSql.c_str());
                        CACHE_LOG( DB_CACHE, "SQL: {}", strProcSql.c_str() );
                    }
                }
            }

            if( ret )
            {
                DISK_LOG(DB_ERROR, "SQL: {}", sql );
                DISK_LOG(DB_ERROR, "query ERROR({}): {}", nErrorNo, mysql_error(m_pMysql) );
                return NULL;
            }

        }
        else
        {
            string strProcSql = "SELECT ";
            char   acNumber[32] = {0};
            for ( int i = 1; i < number+1; i++ )
            {
                memset( acNumber, 0, sizeof(acNumber));
                snprintf(acNumber, sizeof(acNumber), "{}", i);
                string strNumber = acNumber;
                string strPara = "@out_para" + strNumber;
                strProcSql += strPara;
                if ( i < number )
                {
                    strProcSql += ",";
                }
            }

            mysql_query(m_pMysql, strProcSql.c_str());
            CACHE_LOG( DB_CACHE, "SQL: {}", strProcSql.c_str() );
        }

        result = mysql_store_result(m_pMysql);

        rowCount = mysql_affected_rows(m_pMysql);
        fieldCount = mysql_field_count(m_pMysql);
        // end guarded block
    }

    if (!result )
        return NULL;

    QueryResultMysql *queryResult = new QueryResultMysql(result, rowCount, fieldCount);
    if( queryResult == NULL )
    {
        DISK_LOG(DB_ERROR, "while create qureyresult, run out of memory" );
        return NULL;
    }
    queryResult->NextRow();

    return queryResult;
}

bool DatabaseMysql::Execute(const char *sql)
{
    // if (!mMysql)
    //     return false;

    return true;
}

bool DatabaseMysql::DirectExecute(const char* sql )
{
    if (!m_pMysql)
    {
        if (IsConnected() == false)
        {
            return false;
        }
    }

    int ret = mysql_query(m_pMysql, sql );
    if( ret )
    {
        int nErrorNo = mysql_errno( m_pMysql );
        // if mysql has gone, maybe longtime no request or mysql restarted
        if( nErrorNo == CR_SERVER_GONE_ERROR || nErrorNo == CR_SERVER_LOST || nErrorNo == CR_UNKNOWN_ERROR /*|| nErrorNo == CR_PROXY_ERROR*/ )
        {
            Close();

            // reconnect mysql
            if ( Reconnect() == true )  // 重启mysql，再次查询
            {
                CACHE_LOG(DB_CACHE, "reinit mysql success on host [{}]", m_hostInfoString.c_str() );
                // re querey  sql statment
                ret =  mysql_query(m_pMysql, sql);
            }

            if( ret )  // 两次出错就报告取数据失败
            {
                DISK_LOG(DB_ERROR, "SQL: {}", sql );
                DISK_LOG(DB_ERROR, "query ERROR({}): {}", nErrorNo, mysql_error(m_pMysql) );
                return false;
            }
        }
        else
        {
            DISK_LOG(DB_ERROR, "SQL: {}", sql );
            DISK_LOG(DB_ERROR, "query ERROR({}): {}", nErrorNo, mysql_error(m_pMysql) );
            return false;
        }
    }
    else
    {
        // TODO: 加入统计该sql语句执行多长时间
    }
    // end guarded block

    return true;
}

bool DatabaseMysql::RealDirectExecute(const char* sql, unsigned long len)
{
    if (!m_pMysql)
    {
        if (IsConnected() == false)
        {
            return false;
        }
    }

    int ret = mysql_real_query(m_pMysql, sql, len);
    if( ret )
    {
        int nErrorNo = mysql_errno( m_pMysql );
        // if mysql has gone, maybe longtime no request or mysql restarted
        if( nErrorNo == CR_SERVER_GONE_ERROR || nErrorNo == CR_SERVER_LOST || nErrorNo == CR_UNKNOWN_ERROR /*|| nErrorNo == CR_PROXY_ERROR */)
        {

            Close();

            // reconnect mysql
            if ( Reconnect() == true )  // 重启mysql，再次查询
            {
                CACHE_LOG(DB_CACHE, "reinit mysql success on host [{}]", m_hostInfoString.c_str() );
                // re querey  sql statment
                ret =  mysql_real_query(m_pMysql, sql, len);
            }

            if( ret )  // 两次出错就报告取数据失败
            {
                DISK_LOG(DB_ERROR, "SQL: {}", sql );
                DISK_LOG(DB_ERROR, "query ERROR({}): {}", nErrorNo, mysql_error(m_pMysql) );
                return false;
            }
        }
        else
        {
            DISK_LOG(DB_ERROR, "SQL: {}", sql );
            DISK_LOG(DB_ERROR, "query ERROR({}): {}", nErrorNo, mysql_error(m_pMysql) );
            return false;
        }
    }
    else
    {
        // TODO: 加入统计该sql语句执行多长时间
    }
    // end guarded block

    return true;
}

bool DatabaseMysql::_TransactionCmd(const char *sql)
{
    if (mysql_query(m_pMysql, sql))
    {
        DISK_LOG(DB_ERROR, "SQL: {}", sql);
        DISK_LOG(DB_ERROR, "SQL ERROR: {}", mysql_error(m_pMysql));
        return false;
    }
    else
    {
        DISK_LOG(DB_ERROR, "SQL: {}", sql);
    }
    return true;
}

bool DatabaseMysql::BeginTransaction()
{
    return true;
}

bool DatabaseMysql::CommitTransaction()
{
    return false;
}

bool DatabaseMysql::RollbackTransaction()
{
    return true;
}

unsigned long DatabaseMysql::escape_string(char *to, const char *from, unsigned long length)
{
    if (!m_pMysql || !to || !from || !length)
        return 0;

    return(mysql_real_escape_string(m_pMysql, to, from, length));
}

