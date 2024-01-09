//
// Created by just do it on 2023/12/4.
//

#ifndef TINYRPC_CHANNEL_H
#define TINYRPC_CHANNEL_H
#include <string>
#include <google/protobuf/service.h>
#include "tinyRPC/client/client.h"
#include "tinyRPC/client/lb.h"

namespace asio { class io_context; }

namespace tinyRPC {
    class Resolver;
    class Controller;

    class Channel: public google::protobuf::RpcChannel {
    public:
        enum class ConnectMode { DIRECT, REGISTRY };

        Channel(const std::string& registry_endpoints, const std::shared_ptr<Balancer>& lb);

        Channel(const std::string& address, uint16_t port);

        Channel(asio::io_context* ctx, const std::string& address, uint16_t port);

        void WaitForConnect();

        void CallMethod(const google::protobuf::MethodDescriptor* method,
                        google::protobuf::RpcController* controller,
                        const google::protobuf::Message* request,
                        google::protobuf::Message* response,
                        google::protobuf::Closure* done) override;

        ~Channel() override;

    private:
        using ClientPtr = std::unique_ptr<Client>;

        static void Invoke(ClientPtr& client, Controller* controller, RpcRequest& request,
                    google::protobuf::Message* response, google::protobuf::Closure* done);

        ConnectMode mode_;
        ClientPtr client_;
        std::unordered_map<std::string, ClientPtr> clients_;
        std::shared_ptr<Balancer> lb_;
        std::unique_ptr<Resolver> resolver_;
        bool resolved_{false};
    };

}

#endif //TINYRPC_CHANNEL_H
