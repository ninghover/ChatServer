#pragma once
enum EnMsgType
{  
    LOGIN_MSG,  //登录
    LOGIN_MSG_ACK,  //登录的响应
    REG_MSG,    //注册
    REG_MSG_ACK, //注册的响应
    ONE_CHAT_MSG,    //一对一聊天
    ADD_FRIEND_MSG, //添加好友

    CREATE_GROUP_MSG,   //创建群组
    ADD_GROUP_MSG,  //加入群组
    GROUP_CHAT_MSG,  //群内聊天

    LOGINOUT_MSG    //注销消息
    
};