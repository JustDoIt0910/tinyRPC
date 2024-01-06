//
// Created by just do it on 2024/1/6.
//
#include "tinyRPC/registry/etcd.h"
#include <iostream>

using namespace tinyRPC;

int main() {
    EtcdClient c("http://110.40.210.125:2379");
    auto resp = c.Put("foo", "bar");
    if(resp->Ok()) {
        resp = c.Get("foo")
                .Sort(EtcdClient::Target::KEY, EtcdClient::SortOrder::ASCEND)
                .All();
        if(resp->Ok()) {
            for(KV& kv: resp->Results()) {
                std::cout << kv.key_ << " " << kv.value_;
            }
            std::cout << std::endl;
        }
    }
    else {
        std::cout << resp->Error() << std::endl;
    }
}