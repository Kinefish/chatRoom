#include "userModel.hpp"
#include "db.hpp"

bool UserModel::insert(User &user) {
    char sql[1024] = {0};
    sprintf(sql,"insert into user(name,password,state) values('%s', '%s', '%s')",
        user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    
    MySQL mysql;
    if(mysql.connect()) {
        if(mysql.update(sql)) {
            LOG_INFO << "[insert user] success. sql : " << sql;
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;  
        }
    }
    LOG_INFO << "[insert user] error. sql : " << sql;
    return false;
}

User UserModel::query(int id)
{
    char sql[1024] = {0};
    
    sprintf(sql,"select * from user where id = '%d'", id);
    
    MySQL mysql;
    User user;
    if(mysql.connect()) {
        MYSQL_RES* resptr =  mysql.query(sql);
        if(resptr != nullptr) {
            MYSQL_ROW row =  mysql_fetch_row(resptr);
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setPassword(row[2]);
            user.setState(row[3]);
        }
        mysql_free_result(resptr);
    }
    return user;
}

bool UserModel::updateState(User& user)
{
    char sql[1024] = {0};
    sprintf(sql,"update user set state = '%s' where id = '%d'",
        user.getState().c_str(), user.getId());
    MySQL mysql;
    if(mysql.connect() && mysql.update(sql)) {
        return true;  
    }
    LOG_INFO << "[updateState user] error. sql : " << sql;
    return false;
}

bool UserModel::resetState() {
    char sql[1024] = "update user set state = 'offline' where state = 'online'";
    
    MySQL mysql;
    if(mysql.connect() && mysql.update(sql)) {
        return true;  
    }
    LOG_INFO << "[resetState user] error. sql : " << sql;
    return false;
}
