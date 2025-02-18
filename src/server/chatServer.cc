#include "chatServer.hpp"
#include <string>
#include <functional>
#include <iostream>
#include <thread>
#include <json.hpp>
#include <chatService.hpp>

const void ChatServer::start () { server_.start(); }

using namespace std::placeholders;
ChatServer::ChatServer(muduo::net::EventLoop* loop,
    const muduo::net::InetAddress& listenAddr)
    :server_(loop, listenAddr, "chatServer"),
    loop_(loop) {
        server_.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
        server_.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
        server_.setThreadNum(std::thread::hardware_concurrency());
}

//连接回调，连接成功和断开时执行
void ChatServer::onConnection(const muduo::net::TcpConnectionPtr &con) {
    if(!con->connected()) {
        ChatService::instance()->connectException(con);
        con->shutdown();
    }
}

//消息回调
void ChatServer::onMessage(const muduo::net::TcpConnectionPtr &con, muduo::net::Buffer* buf, muduo::Timestamp time) {
    std::string buffer = buf->retrieveAllAsString();
    json js = json::parse(buffer);

    HANDLER msgHandler = ChatService::instance()->getHandler(js["typeId"].get<int>());
    msgHandler(con, js, time);
} 