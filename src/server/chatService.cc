#include "chatService.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <string>
#include "user.hpp"
ChatService *ChatService::instance() {
    static ChatService service;
    return &service;
}

using namespace std::placeholders;
ChatService::ChatService() {
    handlerMap_.insert({TYPE_LOGIN, std::bind(&ChatService::login, this, _1, _2, _3)});
    handlerMap_.insert({TYPE_REG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    handlerMap_.insert({TYPE_MSG_ONE_2_ONE, std::bind(&ChatService::o2oChat, this, _1, _2, _3)});
}

using namespace std::literals;
void ChatService::login(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time) {
    LOG_INFO << "[login service]";
    int id = js["id"].get<int>();
    std::string pwd = js["password"];
    
    User user = userModel_.query(id);
    json respon;
    respon["typeId"] = TYPE_LOGIN_ACK;

    if(user.getId() != id || user.getPassword() != pwd) {
        respon["errNo"] = 1;
        respon["errMsg"] = "user name or password error!";
    } else if(user.getState() == "online"s) {
        respon["errNo"] = 2;
        respon["errMsg"] = "user online, please change password quickly!";
    } else {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            userConnMap_.insert({id, conn});
        }
        user.setState("online");
        userModel_.updateState(user);
        respon["errNo"] = 0;
        respon["name"] = user.getName();
        respon["state"] = user.getState();

        std::vector<std::string> vec = offlineMsgModel_.query(id);
        if(!vec.empty()) {
            respon["offlineMsg"] = vec;
            offlineMsgModel_.remove(id);
        }
    }
    conn->send(respon.dump());
    return;
}

void ChatService::reg(const muduo::net::TcpConnectionPtr &conn, json &js, muduo::Timestamp time) {
    LOG_INFO << "[reg service]";
    std::string name = js["name"];
    std::string pwd = js["password"];
    User user;
    user.setName(name);
    user.setPassword(pwd);

    bool state = userModel_.insert(user);
    json respone;
    respone["typeId"] = TYPE_REG_ACK;
    if(!state) {
        respone["errNo"] = 1;
    } else {
        respone["errNo"] = 0;
        respone["id"] = user.getId();
    }
    conn->send(respone.dump());
}

void ChatService::o2oChat(const muduo::net::TcpConnectionPtr& con, json& js, muduo::Timestamp time) {
    int toId = js["to"].get<int>();
    {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = userConnMap_.find(toId);
        if(it != userConnMap_.end()) {
            it->second->send(js.dump());
            return;
        }  
    }
    offlineMsgModel_.insert(toId, js.dump());
}

#include <algorithm>
void ChatService::connectException(const muduo::net::TcpConnectionPtr& con) {
    User user;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        std::erase_if(userConnMap_, [&](const auto& it) {
            if(it.second == con) {
                user.setId(it.first);
                return true;
            }
            return false;
        });
    }
    if(user.getId() != -1) {
        user.setState("offline");
        userModel_.updateState(user);
    }
}

HANDLER ChatService::getHandler(int typeId) {
    //如果使用handerMap_[id]进行查找，会导致插入一个新的key
    auto it = handlerMap_.find(typeId);
    if(it == handlerMap_.end()) {
        return [=](const muduo::net::TcpConnectionPtr& conn, json &json, muduo::Timestamp time){
            LOG_ERROR << "[msg type]: " << typeId << " cannt found handler.";
        };
    } 
    return handlerMap_[typeId];
}
