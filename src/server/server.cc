//
// Created by just do it on 2023/11/30.
//
#include "server/server.h"
#include "server/session.h"
#include "rpc/codec.h"
#include "rpc/router.h"
#include "asio.hpp"
#include <unordered_map>
#include <utility>

using namespace asio;

namespace tinyRPC {

    class Server::Impl {
    public:
        explicit Impl(const ip::tcp::endpoint& ep, Server* server):
        acceptor_(ioc_),
        server_(server) {
            if(server->Protocol() == RpcProtocol::PROTOBUF) {
                router_ = std::make_unique<ProtobufRpcRouter>();
            }
            acceptor_.open(ep.protocol());
            acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(ep);
            acceptor_.listen();
        }

        void RegisterService(ServicePtr service) { router_->RegisterService(std::move(service)); }

        void RemoveSession(const std::string& session_id) {
            std::shared_ptr<Session> session;
            {
                std::lock_guard<std::mutex> lg(sessions_mu_);
                auto it = sessions_.find(session_id);
                if(it != sessions_.end()) {
                    session = it->second;
                    sessions_.erase(it);
                }
            }
        }

        void StartAccept() {
            acceptor_.async_accept([this] (std::error_code ec, ip::tcp::socket socket) {
                std::unique_ptr<Codec> codec;
                if(server_->Protocol() == RpcProtocol::PROTOBUF) {
                    codec = std::make_unique<ProtobufRpcCodec>();
                }
                session_ptr session = std::make_shared<Session>(server_, ioc_, std::move(socket),
                                                                codec, router_.get());
                session->Start();
                std::lock_guard lg(sessions_mu_);
                sessions_[session->Id()] = session;
                StartAccept();
            });
        }

        void Run() { ioc_.run(); }

    private:

        using session_ptr = std::shared_ptr<Session>;

        io_context ioc_;
        ip::tcp::acceptor acceptor_;
        Server* server_;
        std::unique_ptr<Router> router_;
        std::mutex sessions_mu_;
        std::unordered_map<std::string, session_ptr> sessions_;
    };

    Server::Server(const std::string &address, uint16_t port, RpcProtocol proto):
    protocol_(proto) {
        ip::address addr = ip::make_address(address);
        ip::tcp::endpoint ep(addr, port);
        pimpl_ = std::make_unique<Impl>(ep, this);
        protocol_ = proto;
    }

    Server::Server(uint16_t port, RpcProtocol proto):
    protocol_(proto) {
        ip::tcp::endpoint ep(ip::tcp::v4(), port);
        pimpl_ = std::make_unique<Impl>(ep, this);
    }

    Server::RpcProtocol Server::Protocol() { return protocol_; }

    void Server::RegisterService(ServicePtr service) {
        pimpl_->RegisterService(std::move(service));
    }

    void Server::SetWorkerNum(int num) { workers_.resize(num); }

    void Server::RemoveSession(const std::string &session_id) {
        pimpl_->RemoveSession(session_id);
    }

    void Server::Serve() {
        pimpl_->StartAccept();
        if(workers_.empty()) { pimpl_->Run(); }
        else {
            for(auto& worker : workers_) {
                worker = std::thread([this](){ pimpl_->Run(); });
            }
            for(auto& worker : workers_) { worker.join(); }
        }
    }

    Server::~Server() = default;

}
