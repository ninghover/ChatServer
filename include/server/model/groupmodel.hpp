#pragma once
#include"group.hpp"
#include"db.hpp"
using namespace std;

class GroupModel
{
public:
    bool createGroup(Group&group);

    void addGroup(int userid,int groupid,string role);

    vector<Group>queryGroups(int userid);   //查询用户所在的群组信息

    vector<int>queryGroupUsers(int userid,int groupid); //查询用户所在群的其他用户id，用于发信息
};