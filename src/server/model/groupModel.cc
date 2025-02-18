#include "groupModel.hpp"
#include "db.hpp"

bool GroupModel::createGroup(Group &group) {
    char sql[1024] = {0};
    sprintf(sql,"insert into allgroup(groupname,groupdesc) values('%s', '%s')",
        group.getName().c_str(), group.getDesc().c_str());
    
    MySQL mysql;
    if(mysql.connect()) {
        if(mysql.update(sql)) {
            LOG_INFO << "[insert allgroup] success. sql : " << sql;
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;  
        }
    }
    LOG_INFO << "[insert allgroup] error. sql : " << sql;
    return false;
}

bool GroupModel::addGroup(int userId, int groupId, const std::string role) {
    char sql[1024] = {0};
    sprintf(sql,"insert into groupuser values(%d, %d, '%s')",
        userId, groupId, role.c_str());
    
    MySQL mysql;
    if(mysql.connect()) {
        if(mysql.update(sql)) {
            LOG_INFO << "[insert groupuser] success. sql : " << sql;
            return true;  
        }
    }
    LOG_INFO << "[insert groupuser] error. sql : " << sql;
    return false;
}

/*
1.先查userid有哪些groupid
2.再查groupid在group表中的数据
可以多表查询
*/
std::vector<Group> GroupModel::queryGroups(int userId) {
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from allgroup a join groupuser b on a.id = b.groupid where b.userid = %d", userId);
    
    std::vector<Group> groupVec;
    MySQL mysql;
    if(mysql.connect()) {
        MYSQL_RES* resptr =  mysql.query(sql);
        if(resptr != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(resptr)) != nullptr) {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
        }
        mysql_free_result(resptr);
    }
    for(auto& group: groupVec) {
        sprintf(sql, "select a.id, a.name, a.state, b.grouprole from user a join groupuser b on b.userid = a.id where b.groupid = %d", group.getId());
        MYSQL_RES* resptr =  mysql.query(sql);
        if(resptr != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(resptr)) != nullptr) {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
        }
        mysql_free_result(resptr);
    }
    return groupVec;
}

/*
查询该组的除自己外的全部成员
*/
std::vector<int> GroupModel::queryGroupUsers(int userId, int groupId) {
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where userId != %d and groupId = %d ", userId, groupId);

    std::vector<int> vec;
    MySQL mysql;
    if(mysql.connect()) {
        MYSQL_RES* resptr =  mysql.query(sql);
        if(resptr != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(resptr)) != nullptr) {
                vec.push_back(atoi(row[0]));
            }
        }
        mysql_free_result(resptr);
    }
    return vec;
}
