//
// Created by just do it on 2023/12/11.
//
#include "tinyRPC/server_v2/server.h"
#include "tinyRPC/server_v2/session.h"
#include "tinyRPC/server_v2/gw.h"
#include "tinyRPC/codec/http_codec.h"
#include "tinyRPC/router/http_router.h"
#include "asio.hpp"

namespace tinyRPC {

    class AbstractHttpApiGateway::Impl {
    public:
        Impl(Server* server, uint16_t port, HttpRouter* router):
        server_(server),
        port_(port),
        router_(router),
        ctx_(nullptr) {}

        void Init(asio::io_context& ctx) {
            ctx_ = &ctx;
            acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(ctx);
            asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), port_);
            acceptor_->open(ep.protocol());
            acceptor_->set_option(asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_->bind(ep);
            acceptor_->listen();
            StartAccept();
        }

    private:
        void StartAccept() {
            auto session = server_->GetHttpSession(router_);
            acceptor_->async_accept(session->Socket(), [this, session] (std::error_code ec) {
                server_->AddSession(session);
                StartAccept();
            });
        }

        uint16_t port_;
        Server* server_;
        asio::io_context* ctx_;
        HttpRouter* router_;
        std::unique_ptr<asio::ip::tcp::acceptor> acceptor_;
    };

    AbstractHttpApiGateway::AbstractHttpApiGateway(std::unique_ptr<HttpProtobufMapper> mapper,
                                                   Server *server,
                                                   uint16_t port) {
        router_ = std::make_unique<HttpRouter>(mapper);
        pimpl_ = std::make_unique<Impl>(server, port, router_.get());
        server->SetGateway(this);
    }

    void AbstractHttpApiGateway::Init(asio::io_context &ctx) { pimpl_->Init(ctx); }

    void AbstractHttpApiGateway::RegisterService(std::shared_ptr<google::protobuf::Service> service, bool exec_in_pool) {
        router_->RegisterService(service, exec_in_pool);
    }

    AbstractHttpApiGateway::~AbstractHttpApiGateway() = default;

}