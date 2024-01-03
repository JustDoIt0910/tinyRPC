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
    class Session;
    class ThreadPoolExecutor;

    using ServicePtr = std::shared_ptr<google::protobuf::Service>;
    struct Service {
        ServicePtr service_;
        bool exec_in_pool_;

        Service(ServicePtr service, bool exec_in_pool): service_(service),
        exec_in_pool_(exec_in_pool) {}

        Service(): service_(nullptr), exec_in_pool_(false) {}
    };

    class Router {
    public:
        RpcResponse Route(const RpcRequest& request, std::weak_ptr<Session> session = std::weak_ptr<Session>(),
                ThreadPoolExecutor* executor = nullptr) const;

        void RegisterService(const ServicePtr& service, bool exec_in_pool);

        virtual bool ParseServiceMethod(const std::string& full_name,
                                        std::string& service,
                                        std::string& method) const = 0;

        virtual bool ParseRequestData(google::protobuf::Message* message,
                                      const std::string& data) const = 0;

        virtual bool SerializeResponseData(const google::protobuf::Message* message,
                                           std::string* data) const = 0;

        virtual ~Router() = default;

    private:
        Service GetService(const std::string& service) const;



        std::unordered_map<std::string, Service> service_map_;
    };

}

#endif //TINYRPC_DISPATCHER_H
