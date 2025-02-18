#include "friendModel.hpp"
#include "db.hpp"

bool FriendModel::insert(int userId, int friendId)  {
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", userId, friendId);
    MySQL mysql;
    if(mysql.connect()) {
        if(mysql.update(sql)) {
            LOG_INFO << "[insert friend] success. sql : " << sql;
            return true;  
        }
    }
    LOG_INFO << "[insert friend] error. sql : " << sql;
    return  false;
}

std::vector<User> FriendModel::query(int userId) {
    char sql[1024] = {0};
    sprintf(sql,"select a.id,a.name,a.state from user a join friend b on b.friendid = a.id where b.userid = %d", userId);
    
    std::vector<User> vec;
    MySQL mysql;
    if(mysql.connect()) {
        MYSQL_RES* resptr =  mysql.query(sql);
        if(resptr != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(resptr)) != nullptr) {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
        }
        mysql_free_result(resptr);
    }
    return vec;
}
