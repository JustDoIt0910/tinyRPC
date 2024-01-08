//
// Created by just do it on 2024/1/8.
//

#ifndef TINYRPC_LB_H
#define TINYRPC_LB_H
#include <vector>
#include <string>

namespace tinyRPC {

    class Balancer {
    public:
        virtual void AddEndpoint(const std::string& endpoint) = 0;

        virtual void RemoveEndpoint(const std::string& endpoint) = 0;

        virtual std::string GetEndpoint(const std::string& key) = 0;
    };

}

#endif //TINYRPC_LB_H
