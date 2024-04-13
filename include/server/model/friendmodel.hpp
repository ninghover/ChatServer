#pragma once
#include"user.hpp"
#include<vector>
#include"db.hpp"
#include<string>
using namespace std;

class FriendModel
{
public:
    void add(int userid,int friendid);

    vector<User> query(int userid); //查询userid的所有好友信息
};