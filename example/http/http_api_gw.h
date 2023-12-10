#ifndef TINYRPC_HTTP_API_GW_H
#define TINYRPC_HTTP_API_GW_H
#include "server/server.h"
#include "router/http_router.h"
#include "codec/http_codec.h"

namespace tinyRPC {

    class HttpProtobufMapperImpl: public HttpProtobufMapper {
    public:
        HttpProtobufMapperImpl(): HttpProtobufMapper() {
            mappings_ = {{{"post", "/greet_service/greet"},{"Greeter", "SayHello"}},{{"get", "/user_service/user"},{"UserService", "GetUserById"}}};
        }
    };

    class HttpApiGateway: public AbstractHttpApiGateway {
    public:
        explicit HttpApiGateway(Server* server, uint16_t port = 8080):
        server_(server),
        acceptor_(server->GetAcceptor()) {
            server->SetGateway(this);
            asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), port);
            acceptor_.open(ep.protocol());
            acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(ep);
            acceptor_.listen();
            std::unique_ptr<HttpProtobufMapper> mapper = std::make_unique<HttpProtobufMapperImpl>();
            router_ = std::make_unique<HttpRouter>(mapper);
            StartAccept();
        }

        void RegisterService(std::shared_ptr<google::protobuf::Service> service) override {
            router_->RegisterService(service);
        }

    private:
        void StartAccept() {
            acceptor_.async_accept([this] (std::error_code ec, asio::ip::tcp::socket socket) {
                std::unique_ptr<Codec> codec = std::make_unique<HttpRpcCodec>();
                server_->NewSession(socket, codec, router_.get());
                StartAccept();
            });
        }

        Server* server_;
        asio::ip::tcp::acceptor acceptor_;
        std::unique_ptr<HttpRouter> router_;
    };

}

#endif //TINYRPC_HTTP_API_GW_H