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
        virtual RpcResponse Route(const RpcRequest& request) const = 0;

        void RegisterService(ServicePtr service);

        static bool ParseServiceMethod(const std::string& full_name, std::string& service, std::string& method) ;

        ServicePtr GetService(const std::string& service) const;

        virtual ~Router() = default;

    private:
        std::unordered_map<std::string, ServicePtr> service_map_;
    };

}

#endif //TINYRPC_DISPATCHER_H
