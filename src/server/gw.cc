//
// Created by just do it on 2023/12/11.
//
#include "tinyRPC/server/server.h"
#include "tinyRPC/server/session.h"
#include "tinyRPC/server/gw.h"
#include "tinyRPC/codec/http_codec.h"
#include "tinyRPC/router/http_router.h"
#include "asio.hpp"

namespace tinyRPC {

    class AbstractHttpApiGateway::Impl {
    public:
        Impl(Server* server, uint16_t port, HttpRouter* router):
        server_(server), port_(port), router_(router) {}

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
            acceptor_->async_accept([this] (std::error_code ec, asio::ip::tcp::socket socket) {
                std::unique_ptr<Codec> codec = std::make_unique<HttpRpcCodec>();
                auto session = std::make_shared<Session>(server_, *ctx_, socket, codec, router_);
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

    void AbstractHttpApiGateway::RegisterService(const std::shared_ptr<google::protobuf::Service>& service) {
        router_->RegisterService(service, false);
    }

    AbstractHttpApiGateway::~AbstractHttpApiGateway() = default;

}