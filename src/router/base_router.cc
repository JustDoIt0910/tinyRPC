//
// Created by just do it on 2023/12/5.
//
#include "tinyRPC/router/base_router.h"
#include "tinyRPC/exec/executor.h"
#include "tinyRPC/rpc/message.h"
#include "tinyRPC/rpc/controller.h"
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#ifndef USE_SERVER_V1
#include "tinyRPC/server_v2/session.h"
#else
#include "tinyRPC/server/session.h"
#endif

using namespace google;

namespace tinyRPC {

    void Router::RegisterService(const tinyRPC::ServicePtr& service, bool exec_in_pool) {
        std::string name = service->GetDescriptor()->full_name();
        if(service_map_.find(name) != service_map_.end()) {
            // TODO use log library
            std::cout << "Service " << name << " has already registered" << std::endl;
            return;
        }
        service_map_[name] = Service(service, exec_in_pool);
    }

    Service Router::GetService(const std::string& service) const {
        auto it = service_map_.find(service);
        if(it == service_map_.end()) { return {}; }
        return it->second;
    }

    RpcResponse Router::Route(const RpcRequest &request, std::weak_ptr<Session> session,
                              ThreadPoolExecutor* executor) const {
        RpcResponse response;
        response.msg_id_ = request.msg_id_;
        std::string service_name, method_name;
        if(!ParseServiceMethod(request.full_method_name_, service_name, method_name)) {
            response.ec_ = rpc_error::error_code::RPC_INVALID_METHOD_NAME;
            response.error_detail_ = request.full_method_name_;
            return response;
        }
        Service service = GetService(service_name);
        if(!service.service_) {
            response.ec_ = rpc_error::error_code::RPC_NO_SUCH_SERVICE;
            response.error_detail_ = service_name;
            return response;
        }
        const protobuf::MethodDescriptor* method = service.service_->GetDescriptor()->FindMethodByName(method_name);
        if(!method) {
            response.ec_ = rpc_error::error_code::RPC_NO_SUCH_METHOD;
            response.error_detail_ = method_name;
            return response;
        }
        std::shared_ptr<protobuf::Message> request_message(service.service_->GetRequestPrototype(method).New());
        std::shared_ptr<protobuf::Message> response_message(service.service_->GetResponsePrototype(method).New());
        if(!ParseRequestData(request_message.get(), request.data_)) {
            response.ec_ = rpc_error::error_code::RPC_BAD_DATA;
            return response;
        }
        std::shared_ptr<Controller> controller = std::make_shared<Controller>();
        if(!service.exec_in_pool_) {
            try {
                service.service_->CallMethod(method, controller.get(), request_message.get(), response_message.get(),
                                             nullptr);
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
        }
        else {
#ifndef USE_SERVER_V1
            if(!executor) {
                response.ec_ = rpc_error::error_code::RPC_CALL_ERROR;
                response.error_detail_ = "Executor not found";
                return response;
            }
            response.ec_ = rpc_error::error_code::RPC_IN_PROGRESS;
            try {
                executor->Execute([this, service = service.service_, method, controller,
                                          request_message, response_message, msg_id = response.msg_id_] () mutable
                                          -> std::shared_ptr<RpcResponse> {
                    auto response = std::make_shared<RpcResponse>();
                    response->msg_id_ = std::move(msg_id);
                    try {
                        service->CallMethod(method, controller.get(), request_message.get(), response_message.get(),
                                            nullptr);
                    }
                    catch (const std::exception& e) {
                        response->ec_ = rpc_error::error_code::RPC_CALL_ERROR;
                        response->error_detail_ = e.what();
                        return response;
                    }
                    if(!SerializeResponseData(response_message.get(), &response->data_)) {
                        response->ec_ = rpc_error::error_code::RPC_SERIALIZE_ERROR;
                        return response;
                    }
                    response->ec_ = rpc_error::error_code::RPC_SUCCESS;
                    return response;
                }, [session](std::shared_ptr<RpcResponse> response) {
                    auto session_ptr = session.lock();
                    if(!session_ptr) { return; }
                    session_ptr->RpcCallback(response);
                });
            }
            catch (rpc_error& e) {
                response.ec_ = e.error().code();
            }
#endif
        }
        return response;
    }

}