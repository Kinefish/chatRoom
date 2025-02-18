#ifndef __CHATSERVICE_H__
#define __CHATSERVICE_H__
#include <mutex>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <json.hpp>
#include "userModel.hpp"
#include "offlineMsgModel.hpp"
using json = nlohmann::json;
/*
singleton 即可
*/
using HANDLER = std::function<void(
    const muduo::net::TcpConnectionPtr&, json &, muduo::Timestamp)>;
class ChatService {
public:
    static ChatService* instance();

    void login(const muduo::net::TcpConnectionPtr&, json &, muduo::Timestamp);
    void reg(const muduo::net::TcpConnectionPtr&, json &, muduo::Timestamp);
    void o2oChat(const muduo::net::TcpConnectionPtr&, json &, muduo::Timestamp);
    void connectException(const muduo::net::TcpConnectionPtr&);
    HANDLER getHandler(int typeId);
private:
    ChatService();
private:
    std::unordered_map<int, HANDLER> handlerMap_;
    UserModel userModel_;
    OffLineMsgModel offlineMsgModel_;

    //确保userConnMap_的线程安全
    std::mutex mtx_;
    std::unordered_map<int, muduo::net::TcpConnectionPtr> userConnMap_;
};

#endif