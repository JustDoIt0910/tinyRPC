//
// Created by just do it on 2023/12/2.
//
#include "server.h"

int main() {
    tinyRPC::Server server(9999);
    server.Serve();
}
