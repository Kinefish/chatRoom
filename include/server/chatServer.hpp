#ifndef __CHATSERVER_H__
#define __CHATSERVER_H__

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
class ChatServer
{
public:
    ChatServer(muduo::net::EventLoop*,
                const muduo::net::InetAddress&);
    ~ChatServer() = default;
    const void start ();
private:
    void onConnection(const muduo::net::TcpConnectionPtr&);
    void onMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
private:
    muduo::net::TcpServer server_;
    muduo::net::EventLoop* loop_;
};

#endif