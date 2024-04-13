#include "chatservice.hpp"

ChatService *ChatService::m_instance = new ChatService();

ChatService *ChatService::getInstance()
{
    return m_instance;
}

ChatService::ChatService()
{
    m_handleMap.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)});
    m_handleMap.insert({REG_MSG, bind(&ChatService::reg, this, _1, _2, _3)});
    m_handleMap.insert({ONE_CHAT_MSG, bind(&ChatService::oneChat, this, _1, _2, _3)});
    m_handleMap.insert({ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, _1, _2, _3)});
    m_handleMap.insert({CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, _1, _2, _3)});
    m_handleMap.insert({ADD_GROUP_MSG, bind(&ChatService::addGroup, this, _1, _2, _3)});
    m_handleMap.insert({GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3)});
    m_handleMap.insert({LOGINOUT_MSG, bind(&ChatService::loginout, this, _1, _2, _3)});

    if(m_redis.connect())
    {
        m_redis.init_notify_handle(bind(&ChatService::handleRedisSubscribeMsg,this,_1,_2));
    }
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "LOGIN";
    int id = js["id"].get<int>();
    string password = js["password"];
    User user = m_userModel.query(id);
    json response;
    if (user.getId() == id && user.getPassword() == password)
    {
        if (user.getState() == "online") // 用户已在线
        {
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["errmsg"] = "用户已在线";
        }
        else // 登录成功
        {
            m_redis.subscribe(user.getId());

            {
                lock_guard<mutex> locker(m_connMutex);
                m_connMap[user.getId()] = conn;
            }
            user.setState("online");
            m_userModel.updateState(user);
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            vector<string> vec = m_offlineMsgModel.query(user.getId());
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                m_offlineMsgModel.remove(user.getId());
            }
            vector<User> userVec = m_friendModel.query(user.getId());
            if (!userVec.empty())
            {
                vector<string> tem;
                for (auto &user : userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    tem.push_back(js.dump());
                }
                response["friends"] = tem;
            }
            // 查询用户的群组信息
            vector<Group> groupuserVec = m_groupModel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }

                response["groups"] = groupV;
            }
        }
    }
    else // 用户名或密码错误
    {
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或密码错误";
    }
    conn->send(response.dump());
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "REG";
    User user;
    user.setName(js["name"]);
    user.setPassword(js["password"]);
    json response;
    if (m_userModel.insert(user))
    {
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
    }
    else
    {
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
    }
    conn->send(response.dump());
}

MsgHandle ChatService::getHander(int msgid)
{
    cout << "获取一个handler" << endl;
    auto it = m_handleMap.find(msgid);
    if (it == m_handleMap.end())
    {
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };
    }
    return m_handleMap[msgid];
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> locker(m_connMutex);
        for (auto it = m_connMap.begin(); it != m_connMap.end(); it++)
        {
            if (conn == it->second)
            {
                user.setId(it->first);
                m_connMap.erase(it);
                break;
            }
        }
    }
    user.setState("offline");
    m_redis.unsubscribe(user.getId());
    m_userModel.updateState(user);
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["toid"].get<int>();
    {
        lock_guard<mutex> locker(m_connMutex);
        auto it = m_connMap.find(toid);
        if (it != m_connMap.end())
        {
            it->second->send(js.dump());
            return;
        }
    }
    
    User user=m_userModel.query(toid);
    if(user.getState()=="online")
    {
        m_redis.publish(toid,js.dump());
        return;
    }

    // 存储到离线消息库中
    m_offlineMsgModel.insert(toid, js.dump());
}

void ChatService::reset()
{
    m_userModel.resetState();
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    m_friendModel.add(userid, friendid);
    m_friendModel.add(friendid, userid);
}

void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    Group group(-1, name, desc);
    if (m_groupModel.createGroup(group))
    {
        m_groupModel.addGroup(userid, group.getId(), "creator");
    }
}

void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    m_groupModel.addGroup(userid, groupid, "normal");
}

void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> userIds = m_groupModel.queryGroupUsers(userid, groupid);
    lock_guard<mutex> locker(m_connMutex);
    for (int id : userIds)
    {
        auto it = m_connMap.find(id);
        if (it != m_connMap.end()) // 在线
        {
            it->second->send(js.dump());
            continue;
        }
        else 
        {
            User user=m_userModel.query(id);
            if(user.getState()=="online")
            {
                m_redis.publish(id,js.dump());
                continue;
            }
            //离线
            m_offlineMsgModel.insert(id, js.dump());
        }
    }
}

void ChatService::loginout(const TcpConnectionPtr&conn,json&js,Timestamp time)
{
    int userid=js["id"].get<int>();
    {
        lock_guard<mutex>locker(m_connMutex);
        auto it=m_connMap.find(userid);
        if(it!=m_connMap.end())
        {
            m_connMap.erase(it);
        }
    }
    User user(userid,"","","offline");
    m_redis.unsubscribe(userid);
    m_userModel.updateState(user);
}

void ChatService::handleRedisSubscribeMsg(int channel,string message)
{
    lock_guard<mutex>lock(m_connMutex);
    auto it=m_connMap.find(channel);
    if(it!=m_connMap.end())
    {
        it->second->send(message);
        return ;
    }

    m_offlineMsgModel.insert(channel,message);
}