//
// Created by just do it on 2024/1/4.
//

#ifndef TINYRPC_REGISTRY_H
#define TINYRPC_REGISTRY_H
#include "tinyRPC/registry/etcd.h"
#include "tinyRPC/client/lb.h"
#include "asio/asio.hpp"

using namespace asio::ip;

namespace tinyRPC {

    class Registry {
    public:
        const static std::string prefix;

        static std::string GetInterfaceIP();

        Registry(const std::string& etcd_endpoints, const tcp::endpoint& endpoint, int ttl);

        Registry(std::vector<std::string> etcd_endpoints, const tcp::endpoint& endpoint, int ttl);

        void Register(const std::string& service);

    private:
        EtcdClient client_;
        std::string addr_;
        int ttl_;
    };

    class Resolver {
    public:
        Resolver(const std::string& endpoints, const std::shared_ptr<Balancer>& balancer);

        void Resolve(const std::string& service);

    private:
        void HandleServerListChange(EventList& events);
        EtcdClient client_;
        std::shared_ptr<Balancer> balancer_;
        std::unordered_map<std::string, std::string> servers_;
    };

}

#endif //TINYRPC_REGISTRY_H
