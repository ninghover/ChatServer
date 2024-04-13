#include"db.hpp"

static string server="127.0.0.1";
static string user="root";
static string password="000914";
static string dbname="chat";

MySQL::MySQL()
{
    m_conn=mysql_init(nullptr);
}

MySQL::~MySQL()
{
    if(m_conn!=nullptr)
    {
        mysql_close(m_conn);
    }
}

bool MySQL::connect()
{
    MYSQL*p=mysql_real_connect(m_conn,server.c_str(),user.c_str(),password.c_str(),dbname.c_str(),3306,nullptr,0);
    
    if(p!=nullptr)
    {
        mysql_query(m_conn,"set names utf8");
        LOG_INFO<<"connect mysql success!";
    }
    else
    {
        LOG_INFO<<"connect mysql fail!";
    }
    return p!=nullptr;
}

bool MySQL::update(string sql)
{
    if(mysql_query(m_conn,sql.c_str()))
    {
        LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<sql<<"更新失败";
        return false;
    }
    return true;
}

MYSQL_RES*MySQL::query(string sql)
{
    if(mysql_query(m_conn,sql.c_str()))
    {
        LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<"查询失败";
        return nullptr;
    }
    return mysql_use_result(m_conn);
}

MYSQL*MySQL::getConnection()
{
    return m_conn;
}