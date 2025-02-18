#include "chatServer.hpp"
#include <iostream>
#include "chatService.hpp"
#include <signal.h>

void resetHandler(int) {
    ChatService::instance()->reset();
    exit(0);
}
int main() {
    signal(SIGINT, resetHandler);
    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr);
    server.start();
    loop.loop();
    return 0;
}