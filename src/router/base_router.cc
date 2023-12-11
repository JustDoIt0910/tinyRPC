//
// Created by just do it on 2023/12/5.
//
#include "tinyRPC/router/base_router.h"
#include "tinyRPC/rpc/message.h"
#include "tinyRPC/rpc/controller.h"
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

using namespace google;

namespace tinyRPC {

    void Router::RegisterService(const tinyRPC::ServicePtr& service) {
        std::string name = service->GetDescriptor()->full_name();
        if(service_map_.find(name) != service_map_.end()) {
            // TODO use log library
            std::cout << "Service " << name << " has already registered" << std::endl;
            return;
        }
        service_map_[name] = service;
    }

    ServicePtr Router::GetService(const std::string& service) const {
        auto it = service_map_.find(service);
        if(it == service_map_.end()) { return ServicePtr{}; }
        return it->second;
    }

    RpcResponse Router::Route(const RpcRequest &request) const {
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
        if(!ParseRequestData(request_message.get(), request.data_)) {
            response.ec_ = rpc_error::error_code::RPC_BAD_DATA;
            return response;
        }
        Controller controller;
        try {
            service->CallMethod(method, &controller,request_message.get(),
                                response_message.get(), nullptr);
        }
        catch (const std::exception& e) {
            response.ec_ = rpc_error::error_code::RPC_CALL_ERROR;
            response.error_detail_ = e.what();
            return response;
        }
        if(!SerializeResponseData(response_message.get(), &response.data_)) {
            response.ec_ = rpc_error::error_code::RPC_SERIALIZE_ERROR;
            return response;
        }
        response.ec_ = rpc_error::error_code::RPC_SUCCESS;
        return response;
    }

}