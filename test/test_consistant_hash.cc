//
// Created by just do it on 2024/1/9.
//
#include "tinyRPC/client/lb.h"
#include <iostream>

using namespace tinyRPC;

int main() {
    ConsistentHashBalancer lb(5);
    lb.AddEndpoint("192.168.43.2");
    lb.AddEndpoint("192.168.3.9");
    lb.AddEndpoint("192.168.40.129");
    lb.AddEndpoint("192.168.3.7");
    lb.AddEndpoint("192.168.43.9");
    lb.AddEndpoint("192.168.40.2");

    std::cout << lb.GetEndpoint("key1") << std::endl;
    std::cout << lb.GetEndpoint("key2") << std::endl;
    std::cout << lb.GetEndpoint("key3") << std::endl;
    std::cout << lb.GetEndpoint("key4") << std::endl;
    std::cout << lb.GetEndpoint("key5") << std::endl;

    std::cout << std::endl;

    std::cout << lb.GetEndpoint("key1") << std::endl;
    std::cout << lb.GetEndpoint("key2") << std::endl;
    std::cout << lb.GetEndpoint("key3") << std::endl;
    std::cout << lb.GetEndpoint("key4") << std::endl;
    std::cout << lb.GetEndpoint("key5") << std::endl;

    lb.RemoveEndpoint("192.168.3.9");
    std::cout << std::endl;

    std::cout << lb.GetEndpoint("key1") << std::endl;
    std::cout << lb.GetEndpoint("key2") << std::endl;
    std::cout << lb.GetEndpoint("key3") << std::endl;
    std::cout << lb.GetEndpoint("key4") << std::endl;
    std::cout << lb.GetEndpoint("key5") << std::endl;
}
