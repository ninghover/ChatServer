#include"groupmodel.hpp"

bool GroupModel::createGroup(Group&group)
{
    char sql[1024]={0};
    sprintf(sql,"insert into allgroup (groupname,groupdesc) values('%s','%s')",group.getName().c_str(),group.getDesc().c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
    
}

void GroupModel::addGroup(int userid,int groupid,string role)   //用户入群
{
    char sql[1024]={0};
    sprintf(sql,"insert into groupuser(userid,groupid,grouprole) values(%d,%d,'%s')",userid,groupid,role.c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

vector<Group> GroupModel::queryGroups(int userid)   //查询用户所在的群信息
{
    //1.先根据用户id查出所在的群的详细信息
    //2.再根据群id查出每个群的人
    char sql[1024]={0};
    sprintf(sql,"select a.id,a.groupname,a.groupdesc from allgroup a,groupuser b where b.groupid=a.id and b.userid=%d ",userid);
    MySQL mysql;
    vector<Group>groupVec;
    if(mysql.connect())
    {
        MYSQL_RES *res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    for(auto&group:groupVec)
    {
        sprintf(sql,"select a.id,a.name,a.state,b.grouprole from user a,groupuser b where a.id=b.userid and b.groupid=%d",group.getId());
        MYSQL_RES*res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                GroupUser groupUser;
                groupUser.setId(atoi(row[0]));
                groupUser.setName(row[1]);
                groupUser.setState(row[2]);
                groupUser.setRole(row[3]);
                group.getUsers().push_back(groupUser);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}

vector<int> GroupModel::queryGroupUsers(int userid,int groupid) //查询用户所在群的其他用户id,用于发信息
{
    char sql[1024]={0};
    sprintf(sql,"select userid from groupuser where groupid=%d and userid!=%d",groupid,userid);
    vector<int>idVec;
    MySQL mysql ;
    if(mysql.connect())
    {
        MYSQL_RES*res=mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}
