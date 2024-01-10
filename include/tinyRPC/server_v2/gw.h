//
// Created by just do it on 2023/12/10.
//

#ifndef TINYRPC_GW_H
#define TINYRPC_GW_H
#include <memory>

namespace google::protobuf { class Service; }
namespace asio { class io_context; }

namespace tinyRPC {

    class Server;
    class HttpRouter;
    class HttpProtobufMapper;

    class AbstractHttpApiGateway {
    public:
        AbstractHttpApiGateway(std::unique_ptr<HttpProtobufMapper>, Server* server, uint16_t port);

        void Init(asio::io_context& main_ctx);

        void RegisterService(std::shared_ptr<google::protobuf::Service> service, bool exec_in_pool);

        virtual ~AbstractHttpApiGateway();

    private:
        class Impl;
        std::unique_ptr<Impl> pimpl_;
        std::unique_ptr<HttpRouter> router_;
    };

}

#endif //TINYRPC_GW_H
