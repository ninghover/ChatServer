#pragma once
#include<vector>
#include<string>
#include"db.hpp"

using namespace std;
class OfflineMsgModel
{
public:
    void insert(int userid,string msg);

    void remove(int userid);

    vector<string>query(int userid);
};