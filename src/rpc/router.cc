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
            response.ec_ = rpc_error::error_code(rpc_error::error_code::RPC_INVALID_METHOD_NAME,
                                                 request.full_method_name_);
            return response;
        }
        ServicePtr service = GetService(service_name);
        if(!service) {
            response.ec_ = rpc_error::error_code(rpc_error::error_code::RPC_NO_SUCH_SERVICE,
                                                 service_name);
            return response;
        }
        const protobuf::MethodDescriptor* method = service->GetDescriptor()->FindMethodByName(method_name);
        if(!method) {
            response.ec_ = rpc_error::error_code(rpc_error::error_code::RPC_NO_SUCH_METHOD,
                                                 method_name);
            return response;
        }
        protobuf::Message* request_message = service->GetRequestPrototype(method).New();
        protobuf::Message* response_message = service->GetResponsePrototype(method).New();
        if(!request_message->ParseFromString(request.data_)) {
            response.ec_ = rpc_error::error_code(rpc_error::error_code::RPC_BAD_DATA);
            return response;
        }
        Controller controller;
        RpcClosure closure([](){});
        try {
            service->CallMethod(method, &controller, request_message, response_message, &closure);
        }
        catch (const std::exception& e) {
            response.ec_ = rpc_error::error_code(rpc_error::error_code::RPC_CALL_ERROR,
                                                 e.what());
            return response;
        }
        if(!response_message->SerializeToString(&response.data_)) {
            response.ec_ = rpc_error::error_code(rpc_error::error_code::RPC_SERIALIZE_ERROR);
            return response;
        }
        delete request_message;
        delete response_message;
        return response;
    }

}