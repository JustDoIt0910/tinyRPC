//
// Created by just do it on 2023/12/4.
//

#ifndef TINYRPC_CHANNEL_H
#define TINYRPC_CHANNEL_H
#include <string>
#include <google/protobuf/service.h>
#include "client.h"

namespace tinyRPC {

    class Channel: public google::protobuf::RpcChannel {
    public:
        Channel(const std::string& address, uint16_t port);

        void CallMethod(const google::protobuf::MethodDescriptor* method,
                        google::protobuf::RpcController* controller,
                        const google::protobuf::Message* request,
                        google::protobuf::Message* response,
                        google::protobuf::Closure* done) override;

    private:
        Client client_;
    };

}

#endif //TINYRPC_CHANNEL_H
