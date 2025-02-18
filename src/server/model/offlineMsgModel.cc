#include "offlineMsgModel.hpp"
#include "db.hpp"

bool OffLineMsgModel::insert(int id, std::string msg) {
    char sql[1024] = {0};
    
    sprintf(sql, "insert into offlinemessage values(%d, '%s')",
        id, msg.c_str());
    MySQL mysql;
    if(mysql.connect()) {
        if(mysql.update(sql)) {
            LOG_INFO << "[insert offlineMsg] success. sql : " << sql;
            return true;  
        }
    }
    LOG_INFO << "[insert offlineMsg] error. sql : " << sql;
    return  false;
}

std::vector<std::string> OffLineMsgModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql,"select message from offlinemessage where userid = %d", id);
    
    std::vector<std::string> vec;
    MySQL mysql;
    if(mysql.connect()) {
        MYSQL_RES* resptr =  mysql.query(sql);
        if(resptr != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(resptr)) != nullptr) {
                vec.push_back(row[0]);
            }
        }
        mysql_free_result(resptr);
    }
    return vec;
}

bool OffLineMsgModel::remove(int id) {
    char sql[1024] = {0};
    sprintf(sql,"delete from offlinemessage where userid = '%d'", id);
    
    MySQL mysql;
    if(mysql.connect()) {
        if(mysql.update(sql)) {
            LOG_INFO << "[delete offlineMsg] success. sql : " << sql;
            return true;  
        }
    }
    LOG_INFO << "[delete offlineMsg] error. sql : " << sql;
    return  false;
}
