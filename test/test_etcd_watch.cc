//
// Created by just do it on 2024/1/6.
//
#include "tinyRPC/registry/etcd.h"
#include <iostream>

using namespace tinyRPC;

void handle_event(EventList& events) {
    for(WatchEvent& e: events) {
        std::cout << e.kv.key << " " << e.kv.value << std::endl;
    }
}

int main() {
    EtcdClient c("http://110.40.210.125:2379");
    auto resp = c.WatchPrefix("foo/").HandleEvent(handle_event);
    if(resp->Ok()) {
        std::cout << "watch ok" << std::endl;
    }
    else {
        std::cout << resp->Error() << std::endl;
    }

    while (true);
}