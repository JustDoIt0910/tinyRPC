//
// Created by just do it on 2023/12/3.
//

#ifndef TINYRPC_CLIENT_H
#define TINYRPC_CLIENT_H
#include <memory>
#include <future>
#include "rpc/message.h"

namespace tinyRPC {

    class Client {
    public:
        Client(const std::string& address, uint16_t port);

        std::future<RpcResponse> Call(const RpcRequest& request);

        void SetConnectTimeout(int milliseconds);

        ~Client();

    private:
        class Impl;
        std::unique_ptr<Impl> pimpl_;
    };

}

#endif //TINYRPC_CLIENT_H
