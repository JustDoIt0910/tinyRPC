//
// Created by just do it on 2023/12/1.
//

#ifndef TINYRPC_SESSION_H
#define TINYRPC_SESSION_H
#include "asio.hpp"

using namespace asio;

namespace tinyRPC {
    class Server;
    class Codec;
    class Router;

class Session: public std::enable_shared_from_this<Session>{
    public:
        Session(io_context& ioc, std::unique_ptr<Codec>& codec, Router* router);

        void Start();

        ip::tcp::socket& Socket();

        std::string& Id();

    private:
        void DoRead();

        io_context& ioc_;
        std::unique_ptr<Codec> codec_;
        Router* router_;
        ip::tcp::socket socket_;
        io_context::strand read_strand_;
        io_context::strand write_strand_;
        std::string id_;
    };

}

#endif //TINYRPC_SESSION_H
