//
// Created by just do it on 2023/12/2.
//

#ifndef TINYRPC_DISPATCHER_H
#define TINYRPC_DISPATCHER_H
#include <memory>
#include <unordered_map>
#include <google/protobuf/service.h>

namespace tinyRPC {

    struct RpcRequest;
    struct RpcResponse;

    using ServicePtr = std::shared_ptr<google::protobuf::Service>;

    class Router {
    public:
        RpcResponse Route(const RpcRequest& request) const;

        void RegisterService(const ServicePtr& service);

        virtual bool ParseServiceMethod(const std::string& full_name,
                                        std::string& service,
                                        std::string& method) const = 0;

        virtual bool ParseRequestData(google::protobuf::Message* message,
                                      const std::string& data) const = 0;

        virtual bool SerializeResponseData(google::protobuf::Message* message,
                                           std::string* data) const = 0;

        ServicePtr GetService(const std::string& service) const;

        virtual ~Router() = default;

    private:
        std::unordered_map<std::string, ServicePtr> service_map_;
    };

}

#endif //TINYRPC_DISPATCHER_H
