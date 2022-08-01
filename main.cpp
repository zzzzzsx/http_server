//
// Created by zsx29 on 2022/8/1.
//
#include "server/EventLoop.h"
#include "server/Server.h"

int main() {
    auto *loop = new EventLoop();
    auto *server = new Server(loop);
    loop->loop();
    return 0;
}
