//
// Created by just do it on 2023/12/2.
//
#include "rpc/router.h"
#include "rpc/message.h"
#include "rpc/controller.h"
#include "rpc/closure.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace google;

namespace tinyRPC {

    RpcResponse ProtobufRpcRouter::Route(const RpcRequest &request) const {
        RpcResponse response;
        response.msg_id_ = request.msg_id_;
        std::string service_name, method_name;
        if(!ParseServiceMethod(request.full_method_name_, service_name, method_name)) {
            response.ec_ = rpc_error::error_code::RPC_INVALID_METHOD_NAME;
            response.error_detail_ = request.full_method_name_;
            return response;
        }
        ServicePtr service = GetService(service_name);
        if(!service) {
            response.ec_ = rpc_error::error_code::RPC_NO_SUCH_SERVICE;
            response.error_detail_ = service_name;
            return response;
        }
        const protobuf::MethodDescriptor* method = service->GetDescriptor()->FindMethodByName(method_name);
        if(!method) {
            response.ec_ = rpc_error::error_code::RPC_NO_SUCH_METHOD;
            response.error_detail_ = method_name;
            return response;
        }
        std::unique_ptr<protobuf::Message> request_message(service->GetRequestPrototype(method).New());
        std::unique_ptr<protobuf::Message> response_message(service->GetResponsePrototype(method).New());
        if(!request_message->ParseFromString(request.data_)) {
            response.ec_ = rpc_error::error_code::RPC_BAD_DATA;
            return response;
        }
        Controller controller;
        RpcClosure closure([](){});
        try {
            service->CallMethod(method, &controller,request_message.get(),
                                response_message.get(), &closure);
        }
        catch (const std::exception& e) {
            response.ec_ = rpc_error::error_code::RPC_CALL_ERROR;
            response.error_detail_ = e.what();
            return response;
        }
        if(!response_message->SerializeToString(&response.data_)) {
            response.ec_ = rpc_error::error_code::RPC_SERIALIZE_ERROR;
            return response;
        }
        response.ec_ = rpc_error::error_code::RPC_SUCCESS;
        return response;
    }

}