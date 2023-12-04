//
// Created by just do it on 2023/11/30.
//
#include "server/server.h"
#include "server/session.h"
#include "rpc/codec.h"
#include "asio.hpp"
#include <unordered_map>

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
        using session_ptr = std::shared_ptr<Session>;

        void StartAccept() {
            std::unique_ptr<Codec> codec;
            if(server_->Protocol() == RpcProtocol::PROTOBUF) {
                codec = std::make_unique<ProtobufRpcCodec>();
            }
            session_ptr session = std::make_shared<Session>(ioc_, codec, server_);
            acceptor_.async_accept(session->Socket(), [this, session] (std::error_code ec) {
                session->Start();
                sessions_[session->Id()] = session;
                StartAccept();
            });
        }

        io_context ioc_;
        ip::tcp::acceptor acceptor_;
        Server* server_;
        std::unordered_map<std::string, session_ptr> sessions_;
    };

    Server::Server(const std::string &address, uint16_t port, RpcProtocol proto) {
        ip::address addr = ip::make_address(address);
        ip::tcp::endpoint ep(addr, port);
        pimpl_ = std::make_unique<Impl>(ep, this);
        protocol_ = proto;
    }

    Server::Server(uint16_t port, RpcProtocol proto) {
        ip::tcp::endpoint ep(ip::tcp::v4(), port);
        pimpl_ = std::make_unique<Impl>(ep, this);
        protocol_ = proto;
    }

    Server::RpcProtocol Server::Protocol() { return protocol_; }

    void Server::Serve() {
        pimpl_->Run();
    }

    Server::~Server() = default;
}
