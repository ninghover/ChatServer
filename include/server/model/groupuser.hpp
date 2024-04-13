#pragma once
#include"user.hpp"

class GroupUser:public User
{
public:
    void setRole(string role)
    {
        m_role=role;
    }
    string getRole()
    {
        return m_role;
    }
private:
    string m_role;
};
