#pragma once
#include "public.hpp"
#include <muduo/net/TcpConnection.h>
#include<muduo/base/Logging.h>
#include<muduo/base/Logging.h>
#include "json.hpp"
#include"usermodel.hpp"
#include"offlinemsgmodel.hpp"
#include"friendmodel.hpp"
#include"groupmodel.hpp"
#include"redis.hpp"
#include <functional>
#include<iostream>
#include <map>
#include<mutex>
using namespace muduo;
using namespace muduo::net;
using namespace std;
using json = nlohmann::json;

using MsgHandle = function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>;

class ChatService
{
public:
    static ChatService *getInstance();
    ChatService(const ChatService &) = delete;
    ChatService &operator=(const ChatService &) = delete;
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    MsgHandle getHander(int msgId);
    void clientCloseException(const TcpConnectionPtr&conn);
    void oneChat(const TcpConnectionPtr&conn,json&js,Timestamp time);
    void reset();
    void addFriend(const TcpConnectionPtr&conn,json&js,Timestamp time);
    void createGroup(const TcpConnectionPtr&conn,json&js,Timestamp time);
    void addGroup(const TcpConnectionPtr&conn,json&js,Timestamp time);
    void groupChat(const TcpConnectionPtr&conn,json&js,Timestamp time);
    void loginout(const TcpConnectionPtr&conn,json&js,Timestamp time);
    void handleRedisSubscribeMsg(int channel,string message);

private:
    ChatService();
    static ChatService *m_instance;
    map<int, MsgHandle> m_handleMap;
    UserModel m_userModel;
    OfflineMsgModel m_offlineMsgModel;
    FriendModel m_friendModel;
    GroupModel m_groupModel;
    Redis m_redis;
    map<int,TcpConnectionPtr>m_connMap;
    mutex m_connMutex;
};