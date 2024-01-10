//
// Created by just do it on 2024/1/4.
//
#include "tinyRPC/registry/registry.h"
#include "tinyRPC/rpc/error.h"
#include "tinyRPC/comm/endian.h"
#include "tinyRPC/comm/string_util.h"
#include <ifaddrs.h>
#include <iostream>

namespace tinyRPC {

    const std::string Registry::prefix = "/tinyRPC";

    std::vector<std::string> SplitEndpoints(const std::string& endpoints) {
        std::vector<std::string> eps;
        StringUtil::Split(endpoints, ";", eps);
        return eps;
    }

    std::string Registry::GetInterfaceIP() {
        ifaddrs* interfaces = nullptr;
        if(::getifaddrs(&interfaces) < 0) {
            throw std::runtime_error(strerror(errno));
        }
        std::string ip;
        for(ifaddrs* ifa = interfaces; ifa != nullptr; ifa = ifa->ifa_next) {
            if(ifa->ifa_addr->sa_family == AF_INET) {
                auto *addr = reinterpret_cast<sockaddr_in *>(ifa->ifa_addr);
                in_addr_t addr_h = NetworkToHost(addr->sin_addr.s_addr);
                if ((addr_h & 0xFF000000) == 0x7F000000) { continue; }
                std::string interface_name(ifa->ifa_name);
                std::cout << "using ip address of interface: " << interface_name << std::endl;
                char buf[INET_ADDRSTRLEN];
                if (!inet_ntop(AF_INET, &addr->sin_addr, buf, sizeof(buf))) {
                    freeifaddrs(interfaces);
                    throw std::runtime_error(strerror(errno));
                }
                ip.assign(buf);
                break;
            }
        }
        freeifaddrs(interfaces);
        return ip;
    }

    Registry::Registry(const std::string &etcd_endpoints, const tcp::endpoint& endpoint, int ttl):
    Registry(SplitEndpoints(etcd_endpoints), endpoint, ttl) {}

    Registry::Registry(std::vector<std::string> etcd_endpoints, const tcp::endpoint &endpoint, int ttl):
    client_(std::move(etcd_endpoints)), ttl_(ttl) {
        if(endpoint.address().is_unspecified()) {
            addr_ = GetInterfaceIP();
        }
        else {
            addr_ = endpoint.address().to_string();
        }
        addr_ += ":" + std::to_string(endpoint.port());
    }

    void Registry::Register(const std::string &service) {
        std::string key = prefix + "/" + service + "/" + addr_;
        auto resp = client_.GrantLease(ttl_);
        if(!resp) {
            throw etcd_error("Could not connect to registry");
        }
        if(!resp->Ok()) {
            throw etcd_error(resp->Message());
        }
        int64_t lease = resp->Lease().id;
        resp = client_.Put(key, addr_, lease);
        if(!resp) {
            throw etcd_error("Could not connect to registry");
        }
        if(!resp->Ok()) {
            throw etcd_error(resp->Message());
        }
        client_.Keepalive(lease);
    }

    Resolver::Resolver(const std::string& endpoints, const std::shared_ptr<Balancer>& balancer):
    client_(SplitEndpoints(endpoints)), balancer_(balancer) {}

    void Resolver::Resolve(const std::string &service) {
        std::string key = Registry::prefix + "/" + service;
        auto resp = client_.GetPrefix(key).All();
        if(!resp) {
            throw etcd_error("Could not connect to registry");
        }
        if(!resp->Ok()) {
            throw etcd_error(resp->Message());
        }
        auto& server_list = resp->Values();
        for(const KV& server: server_list) {
            servers_[server.key] = server.value;
            balancer_->AddEndpoint(server.value);
        }
        client_.WatchPrefix(key).HandleEvent([this](EventList& events) {
            HandleServerListChange(events);
        });
    }

    void Resolver::HandleServerListChange(EventList &events) {
        for(const WatchEvent& event: events) {
            if(event.type == WatchEvent::Type::PUT) {
                servers_[event.kv.key] = event.kv.value;
                balancer_->AddEndpoint(event.kv.value);
            }
            else {
                auto it = servers_.find(event.kv.key);
                if(it != servers_.end()) {
                    balancer_->RemoveEndpoint((*it).second);
                    servers_.erase(it);
                }
            }
        }
    }

}