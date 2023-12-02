//
// Created by just do it on 2023/11/30.
//
#include "server.h"
#include "asio.hpp"
#include "session.h"
#include <iostream>

using namespace asio;

namespace tinyRPC {

    class Server::Impl {
    public:
        explicit Impl(const ip::tcp::endpoint& ep, Server* server):
        acceptor_(ioc_),
        server_(server) {
            acceptor_.open(ep.protocol());
            acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(ep);
            acceptor_.listen();
        }

        void Run() {
            StartAccept();
            ioc_.run();
        }

    private:
        void StartAccept() {
            std::shared_ptr<Session> session = std::make_shared<Session>(ioc_, server_);
            acceptor_.async_accept(session->Socket(), [this, session] (std::error_code ec) {
                std::cout << session->Socket().remote_endpoint().address().to_string() << std::endl;
                StartAccept();
            });
        }

        io_context ioc_;
        ip::tcp::acceptor acceptor_;
        Server* server_;
    };

    Server::Server(const std::string &address, uint16_t port) {
        ip::address addr = ip::make_address(address);
        ip::tcp::endpoint ep(addr, port);
        pimpl_ = std::make_unique<Impl>(ep, this);
    }

    Server::Server(uint16_t port) {
        ip::tcp::endpoint ep(ip::tcp::v4(), port);
        pimpl_ = std::make_unique<Impl>(ep, this);
    }

    void Server::Serve() {
        pimpl_->Run();
    }

    Server::~Server() = default;
}

