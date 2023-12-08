//
// Created by just do it on 2023/12/4.
//
#include "client/channel.h"
#include "rpc/message.h"
#include "rpc/controller.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace tinyRPC {

    Channel::Channel(const std::string &address, uint16_t port): client_(address, port) {}

    Channel::Channel(asio::io_context *ctx, const std::string &address, uint16_t port):
    client_(ctx, address, port) {}

    void Channel::WaitForConnect() { client_.WaitForConnect(); }

    void Channel::CallMethod(const google::protobuf::MethodDescriptor *method,
                        google::protobuf::RpcController *controller,
                        const google::protobuf::Message *request,
                        google::protobuf::Message *response,
                        google::protobuf::Closure *done) {
        auto rpc_controller = dynamic_cast<Controller*>(controller);
        RpcRequest req_msg;
        req_msg.msg_id_ = rpc_controller->RequestId();
        req_msg.full_method_name_ = method->full_name();
        if(!request->SerializeToString(&req_msg.data_)) {
            // TODO handle error
        }
        auto timeout = rpc_controller->Timeout();
        if(timeout) {
            client_.SetConnectTimeout(*timeout);
        }
        if(rpc_controller->Async()) { client_.Call(req_msg, done); }
        else {
            auto fu = client_.Call(req_msg);
            RpcResponse resp = fu.get();
            response->ParseFromString(resp.data_);
        }
    }
}