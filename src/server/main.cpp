#include "chatServer.hpp"
#include <iostream>

int main() {
    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr);
    server.start();
    loop.loop();
    return 0;
}