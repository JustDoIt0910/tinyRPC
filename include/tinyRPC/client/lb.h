//
// Created by just do it on 2024/1/8.
//

#ifndef TINYRPC_LB_H
#define TINYRPC_LB_H
#include <vector>
#include <string>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <random>

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
        std::mutex mu_;
        std::unordered_set<std::string> endpoints_;
        std::unordered_set<std::string>::const_iterator cur_;
    };

    class RandomBalancer: public Balancer {
    public:
        RandomBalancer();

        void AddEndpoint(const std::string& endpoint) override;

        void RemoveEndpoint(const std::string& endpoint) override;

        std::string GetEndpoint(const std::string& key) override;

    private:
        std::mutex mu_;
        std::mt19937 engine_;
        std::vector<std::string> endpoints_;
    };

    class ConsistentHashBalancer: public Balancer {
    public:
        explicit ConsistentHashBalancer(int virtual_nodes = 10);

        void AddEndpoint(const std::string& endpoint) override;

        void RemoveEndpoint(const std::string& endpoint) override;

        std::string GetEndpoint(const std::string& key) override;

    private:
        std::mutex mu_;
        int virtual_nodes_;
        std::vector<uint32_t> hash_circle_;
        std::unordered_map<uint32_t, std::string> servers_;
    };

}

#endif //TINYRPC_LB_H
