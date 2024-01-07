//
// Created by just do it on 2024/1/6.
//
#include "tinyRPC/registry/etcd.h"
#include <iostream>

using namespace tinyRPC;

int main() {
    EtcdClient c("http://110.40.210.125:2379");
    auto resp = c.Put("test/", "abc");
    resp = c.Put("test/1", "def");
    resp = c.Put("test/1/2", "ghi");
    if(resp->Ok()) {
        resp = c.GetPrefix("test/")
                .Sort(EtcdClient::Target::KEY, EtcdClient::SortOrder::DESCEND)
                .All();
        if(resp->Ok()) {
            for(KV& kv: resp->Values()) {
                std::cout << kv.key << " " << kv.value;
                std::cout << std::endl;
            }
        }
    }
    else {
        std::cout << resp->Error() << std::endl;
    }
}