//
// Created by just do it on 2023/12/1.
//
#include "session.h"

namespace tinyRPC {
    Session::Session(asio::io_context &ioc, tinyRPC::Server *server):
    ioc_(ioc), server_(server), socket_(ioc_) {

    }

    ip::tcp::socket &Session::Socket() { return socket_; }
}