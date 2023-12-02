//
// Created by just do it on 2023/12/1.
//

#ifndef TINYRPC_SESSION_H
#define TINYRPC_SESSION_H
#include "server.h"
#include "asio.hpp"

using namespace asio;

namespace tinyRPC {

    class Session {
    public:
        Session(io_context& ioc, Server* server);

        ip::tcp::socket& Socket();

    private:
        io_context& ioc_;
        Server* server_;
        ip::tcp::socket socket_;
    };

}

#endif //TINYRPC_SESSION_H
