# ChatServer
c++集群聊天服务器，基于cmake构建编译环境，muduo库实现网络通信，json数据传输，nginx负载均衡，redis消息队列实现跨服务器通信

## 功能描述
1. 注册、登录、注销
2. 添加好友（不需要对方同意）
3. 好友聊天
4. 创建群聊、加入群
5. 群内聊天
6. 离线消息记录
   
## 运行方式
进入`./build`文件夹，执行`./build.sh`,在`./bin`文件夹下生成`ChatServer`和`ChatClient`文件

进入`./bin`文件夹，执行`./ChatServer 127.0.0.1 9528`启动服务器

进入`./bin`文件夹，执行`./ChatClient 127.0.0.1 9527`启动客户端

## nginx配置
nginx监听`9527`端口，代理的3个服务器端口号为`9528`,`9529`,`9530`

nginx启动方式: `cd /usr/local/nginx/sbin` ,`./nginx`

nginx停止方式: `cd /usr/local/nginx/sbin` ,`./nginx -s stop`

重新加载配置文件: `cd /usr/local/nginx/sbin` ,`./nginx -s reload`

nginx配置文件位置: `/usr/local/nginx/conf/nginx.conf`
