//
// Created by just do it on 2024/1/7.
//
#include "tinyRPC/registry/etcd.h"
#include <iostream>

using namespace tinyRPC;
using namespace std::chrono_literals;

int main() {
    EtcdClient c("http://110.40.210.125:2379");
//    c.Put("foo", "bar1");
//    c.Put("foo/", "bar2");
//    c.Put("foo/1", "bar3");
//    c.Put("foo/1/2", "bar4");

    auto resp = c.GrantLease(5, 1);
    if(resp->Ok()) {
        int64_t lease_id = resp->Lease().id;
        c.Put("foo", "bar", lease_id);
        resp = c.LeaseTTL(lease_id);
        if(resp->Ok()) {
            Lease& lease = resp->Lease();
            std::cout << "lease: " << lease.id << std::endl;
            std::cout << "granted ttl: " << lease.granted_ttl << std::endl;
            std::cout << "ttl: " << lease.ttl << std::endl;
            std::cout << "keys:" << std::endl;
            for(auto& key: lease.attacked_keys) {
                std::cout << key << std::endl;
            }
        }
    }
}