//
// Created by just do it on 2023/12/3.
//

#ifndef TINYRPC_CLIENT_H
#define TINYRPC_CLIENT_H
#include <memory>
#include <future>
#include "rpc/message.h"

namespace google::protobuf { class Closure; }
namespace asio { class io_context; }

namespace tinyRPC {

    class Client {
    public:
        Client(const std::string& address, uint16_t port);

        Client(asio::io_context* ctx, const std::string& address, uint16_t port);

        void SetConnectTimeout(int milliseconds);

        void WaitForConnect();

        std::future<RpcResponse> Call(const RpcRequest& request);

        void Call(const RpcRequest& request, google::protobuf::Closure* closure);

        ~Client();

    private:
        class Impl;
        std::unique_ptr<Impl> pimpl_;
    };

}

#endif //TINYRPC_CLIENT_H
