#ifndef __DB_H__
#define __DB_H__
#include <string>
#include <mysql/mysql.h>
#include <muduo/base/Logging.h>
// 数据库配置信息
static std::string server = "127.0.0.1";
static std::string user = "root";
static std::string password = "123QwQ!@#";
static std::string dbname = "chat";
// 数据库操作类
class MySQL {
public:
// 初始化数据库连接
    MySQL();
// 释放数据库连接资源
    ~MySQL();
// 连接数据库
    bool connect();
// 更新操作
    bool update(std::string sql);
// 查询操作
    MYSQL_RES* query(std::string sql);
    MYSQL* getConnection() const;
private:
    MYSQL *conn_;
};

#endif