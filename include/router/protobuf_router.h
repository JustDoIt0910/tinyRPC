//
// Created by just do it on 2023/12/5.
//

#ifndef TINYRPC_PROTOBUF_ROUTER_H
#define TINYRPC_PROTOBUF_ROUTER_H
#include "base_router.h"

namespace tinyRPC {

    class ProtobufRpcRouter: public Router {
    public:
        bool ParseServiceMethod(const std::string &full_name,
                                std::string& service,
                                std::string& method) const override;

        bool ParseRequestData(google::protobuf::Message* message,
                              const std::string& data) const override;

        bool SerializeResponseData(const google::protobuf::Message* message,
                                   std::string* data) const override;
    };

}

#endif //TINYRPC_PROTOBUF_ROUTER_H
