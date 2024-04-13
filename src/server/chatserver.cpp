#include "chatserver.hpp"
#include <iostream>
using namespace std;
using namespace placeholders;

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg) : m_server(loop, listenAddr, nameArg), m_loop(loop)
{
    m_server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));
    m_server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2,_3));
    m_server.setThreadNum(4);
}

void ChatServer::start()
{
    m_server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        cout << conn->peerAddress().toIpPort() << endl;
    }
    else
    {
        ChatService::getInstance()->clientCloseException(conn);
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{

    string buf = buffer->retrieveAllAsString();

    // 测试，添加json打印代码
    cout << buf << endl;

    json js = json::parse(buf);
    auto msgHandler = ChatService::getInstance()->getHander(js["msgid"].get<int>());
    msgHandler(conn, js, time);
}
