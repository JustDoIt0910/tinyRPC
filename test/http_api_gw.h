//
// Created by just do it on 2023/12/9.
//

#ifndef TINYRPC_HTTP_API_GW_H
#define TINYRPC_HTTP_API_GW_H
#include "server/server.h"
#include <iostream>

namespace tinyRPC {

    class HttpApiGateway {
    public:
        HttpApiGateway(Server* server, uint16_t port = 8080): acceptor_(server->GetAcceptor()) {
            asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), port);
            acceptor_.open(ep.protocol());
            acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(ep);
            acceptor_.listen();

            acceptor_.async_accept([] (std::error_code ec, asio::ip::tcp::socket socket) {

            });
        }

    private:
        asio::ip::tcp::acceptor acceptor_;
    };

}

#endif //TINYRPC_HTTP_API_GW_H
