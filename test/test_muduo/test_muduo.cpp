/*
muduo test 2.0
*/
#include <iostream>
#include <thread>
#include <string>
#include <functional>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
class chatServer
{
public:
    chatServer(muduo::net::EventLoop*,
                const muduo::net::InetAddress&);
    ~chatServer() = default;
    const void start () { server_.start(); };
private:
    void onConnection(const muduo::net::TcpConnectionPtr&);
    void onMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
private:
    muduo::net::TcpServer server_;
    muduo::net::EventLoop* loop_;
};

using namespace std::placeholders;
chatServer::chatServer(muduo::net::EventLoop* loop,
    const muduo::net::InetAddress& listenAddr)
    :server_(loop, listenAddr, "chatServer"),
    loop_(loop) {
        server_.setConnectionCallback(std::bind(&chatServer::onConnection, this, _1));
        server_.setMessageCallback(std::bind(&chatServer::onMessage, this, _1, _2, _3));
        server_.setThreadNum(std::thread::hardware_concurrency());
}

void chatServer::onConnection(const muduo::net::TcpConnectionPtr &con) {
    if(con->connected()) {
        std::cout << con->peerAddress().toIpPort() << "->" << con->localAddress().toIpPort() << "state: online." << std::endl;
    } else {
        std::cout << con->peerAddress().toIpPort() << "->" << con->localAddress().toIpPort() << "state: offline." << std::endl;
        con->shutdown();
    }
}

void chatServer::onMessage(const muduo::net::TcpConnectionPtr &con, muduo::net::Buffer *buf, muduo::Timestamp time) {
    std::string str = buf->retrieveAllAsString();
    std::cout << "[recv data] :" << str << std::endl;
    con->send(str);
}

int main() {
    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr(6000);
    chatServer chat(&loop, addr);
    
    chat.start();
    loop.loop();
    return 0;
}