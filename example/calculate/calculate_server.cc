//
// Created by just do it on 2023/12/5.
//
#include "server/server.h"
#include <thread>
#include <iostream>
#include "asio.hpp"

int main() {
    tinyRPC::Server server(9999);
    server.Serve();
}