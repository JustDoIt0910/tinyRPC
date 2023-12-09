//
// Created by just do it on 2023/11/30.
//
#include "server/server.h"
#include "server/session.h"
#include "codec/protobuf_codec.h"
#include "router/protobuf_router.h"
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
            router_ = std::make_unique<ProtobufRpcRouter>();
            acceptor_.open(ep.protocol());
            acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(ep);
            acceptor_.listen();
        }

        void RegisterService(ServicePtr service) { router_->RegisterService(service); }

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
                std::unique_ptr<Codec> codec = std::make_unique<ProtobufRpcCodec>();
                NewSession(socket, codec, router_.get());
                StartAccept();
            });
        }

        void NewSession(ip::tcp::socket& socket, std::unique_ptr<Codec>& codec, Router* router) {
            session_ptr session = std::make_shared<Session>(server_, ioc_, socket, codec, router);
            session->Start();
            std::lock_guard lg(sessions_mu_);
            sessions_[session->Id()] = session;
        }

        io_context& GetContext() { return ioc_; };

    private:
        using session_ptr = std::shared_ptr<Session>;

        io_context ioc_;
        ip::tcp::acceptor acceptor_;
        Server* server_;
        std::unique_ptr<Router> router_;
        std::mutex sessions_mu_;
        std::unordered_map<std::string, session_ptr> sessions_;
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

    void Server::RegisterService(ServicePtr service) {
        pimpl_->RegisterService(std::move(service));
    }

    void Server::SetWorkerNum(int num) { workers_.resize(num); }

    void Server::RemoveSession(const std::string &session_id) {
        pimpl_->RemoveSession(session_id);
    }

    void Server::Serve() {
        pimpl_->StartAccept();
        io_context& ctx = pimpl_->GetContext();
        if(workers_.empty()) { ctx.run(); }
        else {
            for(auto& worker : workers_) {
                worker = std::thread([&ctx] () { ctx.run(); });
            }
            for(auto& worker : workers_) { worker.join(); }
        }
    }

    ip::tcp::acceptor Server::GetAcceptor() {
        ip::tcp::acceptor acceptor(pimpl_->GetContext());
        return acceptor;
    };

    void Server::NewSession(ip::tcp::socket& socket, std::unique_ptr<Codec>& codec, Router* router) {
        pimpl_->NewSession(socket, codec, router);
    }

    Server::~Server() = default;

}
