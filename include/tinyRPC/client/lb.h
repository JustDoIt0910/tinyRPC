//
// Created by just do it on 2024/1/8.
//

#ifndef TINYRPC_LB_H
#define TINYRPC_LB_H
#include <vector>
#include <string>
#include <unordered_set>

namespace tinyRPC {

    class Balancer {
    public:
        virtual void AddEndpoint(const std::string& endpoint) = 0;

        virtual void RemoveEndpoint(const std::string& endpoint) = 0;

        virtual std::string GetEndpoint(const std::string& key) = 0;
    };

    class RoundRobinBalancer: public Balancer {
    public:
        void AddEndpoint(const std::string& endpoint) override;

        void RemoveEndpoint(const std::string& endpoint) override;

        std::string GetEndpoint(const std::string& key) override;

    private:
        std::unordered_set<std::string> endpoints_;
        std::unordered_set<std::string>::const_iterator cur_;
    };

    class ConsistentHashBalancer: public Balancer {
        
    };

}

#endif //TINYRPC_LB_H
