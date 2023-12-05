//
// Created by just do it on 2023/12/5.
//

#ifndef TINYRPC_ROUTER_H
#define TINYRPC_ROUTER_H
#include "abstract_router.h"

namespace tinyRPC {

    class ProtobufRpcRouter: public Router {
    public:
        RpcResponse Route(const RpcRequest& request) const override;
    };

}

#endif //TINYRPC_ROUTER_H
