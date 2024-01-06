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

    }
    else {
        std::cout << resp->Error() << std::endl;
    }
}