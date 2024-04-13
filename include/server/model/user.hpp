#pragma once
#include<string>
using namespace std;
class  User
{
public:
    User(int id=-1,string name="",string password="",string state="offline")
    {
        m_id=id;
        m_name=name;
        m_password=password;
        m_state=state;
    }
    void setId(int id)
    {
        m_id=id;
    }
    void setName(string name)
    {
        m_name=name;
    }
    void setPassword(string password)
    {
        m_password=password;
    }
    void setState(string state)
    {
        m_state=state;
    }
    int getId()
    {
        return m_id;
    }
    string getName()
    {
        return m_name;
    }
    string getPassword()
    {
        return m_password;
    }
    string getState()
    {
        return m_state;
    }
protected:
    int m_id;
    string m_name;
    string m_password;
    string m_state;
};