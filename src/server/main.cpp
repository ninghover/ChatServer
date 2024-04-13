#include"chatserver.hpp"
#include"chatservice.hpp"
#include<iostream>
#include<signal.h>
using namespace std;
void handle(int)
{
    ChatService::getInstance()->reset();
    exit(0);
}
int main(int argc,char**argv)
{
    if(argc<3)
    {
        cout<<"nginx代理三个server端口,9528,9529,9530.对外暴露9527"<<endl;
        cout<<"exmaple: ./ChatServer 127.0.0.1 9528"<<endl;
        exit(-1);
    }
    
    char *ip=argv[1];
    uint16_t port=atoi(argv[2]);

    signal(SIGINT,handle);

    EventLoop loop;
    InetAddress addr(ip,port);
    ChatServer server(&loop,addr,"ChatServer");

    server.start();
    loop.loop();
    return 0;
}