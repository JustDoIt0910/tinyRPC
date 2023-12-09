//
// Created by just do it on 2023/12/9.
//
#include "http_api_gw.h"

using namespace tinyRPC;

int main() {

    Server server("127.0.0.1", 9999);
    HttpApiGateway gw(&server, 8080);

    server.Serve();
}