#include "chatService.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <string>
#include "user.hpp"
using namespace std::literals;
using namespace std::placeholders;

ChatService *ChatService::instance() {
    static ChatService service;
    return &service;
}

ChatService::ChatService() {
    handlerMap_.insert({TYPE_LOGIN, std::bind(&ChatService::login, this, _1, _2, _3)});
    handlerMap_.insert({TYPE_REG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    handlerMap_.insert({TYPE_MSG_ONE_2_ONE, std::bind(&ChatService::o2oChat, this, _1, _2, _3)});
    handlerMap_.insert({TYPE_GROUP_CREATE, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    handlerMap_.insert({TYPE_GROUP_ADD, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    handlerMap_.insert({TYPE_GROUP_CHAT, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
}

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
        std::vector<User> vec2 = friendModel_.query(id);
        if(!vec2.empty()) {
            std::vector<std::string> friendVec;
            for(auto& it : vec2) {
                json tmpjs;
                tmpjs["id"] = it.getId();
                tmpjs["name"] = it.getName();
                tmpjs["state"] = it.getState();
                friendVec.push_back(tmpjs.dump());
            }
            respon["friends"] = friendVec;
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

/*
        可以过滤非法userId,在userModel_里边添加方法即可，但可能会查一遍数据库
*/
void ChatService::addFriend(const muduo::net::TcpConnectionPtr &con, json &js, muduo::Timestamp time) {
    int userId = js["id"].get<int>();
    int friendId = js["friendId"].get<int>();
    friendModel_.insert(userId, friendId);
}

void ChatService::createGroup(const muduo::net::TcpConnectionPtr &con, json &js, muduo::Timestamp time) {
    
    Group group(-1, js["groupName"], js["groupDesc"]);
    if(groupModel_.createGroup(group)) {
        int userId = js["id"].get<int>();
        groupModel_.addGroup(userId, group.getId(), "creator"s);
    }
}

void ChatService::addGroup(const muduo::net::TcpConnectionPtr &con, json &js, muduo::Timestamp time) {
    groupModel_.addGroup(js["id"].get<int>(), js["groupId"].get<int>(), "normal"s);
}

void ChatService::groupChat(const muduo::net::TcpConnectionPtr &con, json &js, muduo::Timestamp time) {
    int userId = js["id"].get<int>();
    int groupId = js["groupId"].get<int>();
    
    std::vector<int> userIds = groupModel_.queryGroupUsers(userId, groupId);
    
    std::lock_guard<std::mutex> lock(mtx_);
    for(int id: userIds) {
        auto it = userConnMap_.find(id);
        if(it != userConnMap_.end()) {
            it->second->send(js.dump());
        } else {
            offlineMsgModel_.insert(id, js.dump());
        }
    }
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

//server quit execption
/*
1.reset user state
...
*/
void ChatService::reset() {
    std::lock_guard<std::mutex> lock(mtx_);
    userModel_.resetState();
    userConnMap_.clear();
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
