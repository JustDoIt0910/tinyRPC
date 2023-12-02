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

        void start();

        ip::tcp::socket& socket();

        std::string& id();

    private:
        void do_read();

        io_context& ioc_;
        Server* server_;
        ip::tcp::socket socket_;
        std::string id_;
    };

}

#endif //TINYRPC_SESSION_H
