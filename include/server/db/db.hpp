#pragma once
#include<mysql/mysql.h>
#include<string>
#include<muduo/base/Logging.h>
#include<iostream>
using namespace std;

class MySQL
{
public:
    MySQL();    //初始化连接
    ~MySQL();   //释放数据库连接资源
    bool connect(); //连接数据库
    bool update(string sql);    //更新操作
    MYSQL_RES*query(string sql);    //查询操作
    MYSQL*getConnection();  //获取连接

private:
    MYSQL*m_conn;
};
