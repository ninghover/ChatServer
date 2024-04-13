#pragma once
#include<hiredis/hiredis.h>
#include<thread>
#include<functional>
#include<iostream>
#include<string>

using namespace std;

class Redis
{
public:
    Redis();
    ~Redis();

    bool connect();

    bool publish(int channel,string message);

    bool subscribe(int channel);

    bool unsubscribe(int channel);

    //在独立线程中接收订阅通道的信息
    void observer_channel_message();

    //初始化向业务层上报通道消息的回调对象
    void init_notify_handle(function<void(int,string)> fn);

private:
    redisContext*m_publish_context;

    redisContext*m_subscribe_context;

    function<void(int,string)>m_notify_message_handle;
};