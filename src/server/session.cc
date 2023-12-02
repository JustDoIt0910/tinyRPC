//
// Created by just do it on 2023/12/1.
//
#include "server/session.h"
#include <cstdio>

namespace tinyRPC {

    Session::Session(asio::io_context &ioc, tinyRPC::Server *server):
    ioc_(ioc), server_(server), socket_(ioc_) {

    }

    void Session::start() {
        char buf[50] = {0};
        auto ep = socket_.remote_endpoint();
        std::string addr_str = ep.address().to_string();
        sprintf(buf, "%s:%u", addr_str.c_str(), ep.port());
        id_ = std::string(buf);
        do_read();
    }

    ip::tcp::socket& Session::socket() { return socket_; }

    std::string& Session::id() { return id_; }

    void Session::do_read() {

    }
}