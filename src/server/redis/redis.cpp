#include "redis.hpp"

Redis::Redis():m_publish_context(nullptr),m_subscribe_context(nullptr)
{

} 

Redis::~Redis()
{
    if(m_publish_context!=nullptr)
        redisFree(m_publish_context);
    if(m_subscribe_context!=nullptr)
        redisFree(m_subscribe_context);
}

bool Redis::connect()
{
    m_publish_context=redisConnect("127.0.0.1",6379);
    if(m_publish_context==nullptr)
    {
        cout<<"publish redis failed!"<<endl;
        return false;
    }
    m_subscribe_context=redisConnect("127.0.0.1",6379);
    if(m_subscribe_context==nullptr)
    {
        cout<<"subscribe redis failed!"<<endl;
        return false;
    }

    //在单独的线程中监听通道上的事件，有消息给业务层上报
    thread t([&](){
        observer_channel_message();
    });
    t.detach();
    cout<<"connect redis-server success!"<<endl;
    return true;
}

bool Redis::publish(int channel, string message)
{
/**
 * redisCommand主要是三步操作：
 * 1.将命令缓存到本地 -> redisAppendCommand
 * 2.将本地命令发送到redis server上 -> redisBufferWrite
 * 3.本线程阻塞的等待redis server响应 -> redisGetReply
 * 因为publish后马上就能得到服务器的响应,所以publish可以直接调用redisCommand,
 * 而subscribe后不会马上得到服务器响应,除非该订阅通道上有新的消息,所以拆分写
*/
    redisReply*reply=(redisReply*)redisCommand(m_publish_context,"publish %d %s",channel,message.c_str());
    if(reply==nullptr)
    {
        cerr<<"publish command failed!"<<endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool Redis::subscribe(int channel)
{
    //本线程只负责发送命令,不在该线程上阻塞的等待通道消息,在独立线程上调用observe_channel_message接收通道消息
    if(REDIS_ERR==redisAppendCommand(m_subscribe_context,"subscribe %d",channel))
    {
        cerr<<"subscribe command failed!"<<endl;
        return false;
    }
    //redisBufferWrite循环发送缓冲区，发送完毕时done被置为1
    int done=0;
    while(!done)
    {
        if(REDIS_ERR==redisBufferWrite(m_subscribe_context,&done))
        {
            cerr<<"subscribe command failed!"<<endl;
            return false;            
        }
    }
    
    return true;
}

bool Redis::unsubscribe(int channel)
{
    if(REDIS_ERR==redisAppendCommand(m_subscribe_context,"unsubscribe %d",channel))
    {
        cerr<<"unsubscribe command failed!"<<endl;
        return false;
    }
    int done=0;
    while(!done)
    {
        if(REDIS_ERR==redisBufferWrite(m_subscribe_context,&done))
        {
            cerr<<"unsubscribe command failed!"<<endl;
            return false;            
        }
    } 
    return true;    
}

// 在独立线程中接收订阅通道的信息
void Redis::observer_channel_message() 
{
    redisReply *reply=nullptr;
    while(REDIS_OK==redisGetReply(m_subscribe_context,(void **)&reply))
    {
        //订阅收到的消息是一个三元组
        if(reply!=nullptr&&reply->element[2]!=nullptr&&reply->element[2]->str!=nullptr)
        {
            //给业务层上报消息
            m_notify_message_handle(atoi(reply->element[1]->str),reply->element[2]->str);
        }

        freeReplyObject(reply);
    }
}

// 初始化向业务层上报通道消息的回调对象
void Redis::init_notify_handle(function<void(int, string)> fn)
{
    m_notify_message_handle=fn;
}
