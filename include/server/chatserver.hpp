#pragma once
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include"json.hpp"
#include"chatservice.hpp"
using namespace muduo;
using namespace muduo::net;
using namespace std;
using json=nlohmann::json;
class ChatServer
{
public:
    ChatServer(EventLoop*loop,const InetAddress&listenAddr,const string &nameArg);

    void start();

private:
    
    void onConnection(const TcpConnectionPtr&conn);
    void onMessage(const TcpConnectionPtr&conn,Buffer*buffer,Timestamp time);
    TcpServer m_server;
    EventLoop*m_loop;
};