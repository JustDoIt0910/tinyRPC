//
// Created by just do it on 2024/1/8.
//
#include <algorithm>
#include "tinyRPC/client/lb.h"

namespace tinyRPC {

    void RoundRobinBalancer::AddEndpoint(const std::string &endpoint) {
        std::lock_guard<std::mutex> lk(mu_);
        endpoints_.insert(endpoint);
        cur_ = endpoints_.begin();
    }

    void RoundRobinBalancer::RemoveEndpoint(const std::string &endpoint) {
        std::lock_guard<std::mutex> lk(mu_);
        endpoints_.erase(endpoint);
        cur_ = endpoints_.begin();
    }

    std::string RoundRobinBalancer::GetEndpoint(const std::string &key) {
        std::lock_guard<std::mutex> lk(mu_);
        if(cur_ == endpoints_.end()) {
            cur_ = endpoints_.begin();
        }
        if(cur_ == endpoints_.end()) { return {}; }
        return *(cur_++);
    }

    RandomBalancer::RandomBalancer(): engine_(std::random_device()()) {}

    void RandomBalancer::AddEndpoint(const std::string &endpoint) {
        std::lock_guard<std::mutex> lk(mu_);
        endpoints_.push_back(endpoint);
    }

    void RandomBalancer::RemoveEndpoint(const std::string &endpoint) {
        std::lock_guard<std::mutex> lk(mu_);
        endpoints_.erase(std::remove(endpoints_.begin(), endpoints_.end(),endpoint), endpoints_.end());
    }

    std::string RandomBalancer::GetEndpoint(const std::string &key) {
        std::lock_guard<std::mutex> lk(mu_);
        std::uniform_int_distribution<size_t> u(0, endpoints_.size() - 1);
        return endpoints_[u(engine_)];
    }

    ConsistentHashBalancer::ConsistentHashBalancer(int virtual_nodes): virtual_nodes_(virtual_nodes) {}

    void ConsistentHashBalancer::AddEndpoint(const std::string &endpoint) {
        std::lock_guard<std::mutex> lk(mu_);
        uint32_t hash = std::hash<std::string>()(endpoint);
        hash_circle_.push_back(hash);
        servers_[hash] = endpoint;
        for(int i = 1; i <= virtual_nodes_; i++) {
            std::string virtual_node_name = endpoint + "-v" + std::to_string(i);
            hash = std::hash<std::string>()(virtual_node_name);
            hash_circle_.push_back(hash);
            servers_[hash] = endpoint;
        }
        std::sort(hash_circle_.begin(), hash_circle_.end());
    }

    void ConsistentHashBalancer::RemoveEndpoint(const std::string &endpoint) {
        std::lock_guard<std::mutex> lk(mu_);
        uint32_t hash = std::hash<std::string>()(endpoint);
        if(servers_.find(hash) == servers_.end()) { return; }
        servers_.erase(hash);
        std::unordered_set<uint32_t> s;
        std::vector<uint32_t> tmp;
        s.insert(hash);
        for(int i = 1; i <= virtual_nodes_; i++) {
            std::string virtual_node_name = endpoint + "-v" + std::to_string(i);
            hash = std::hash<std::string>()(virtual_node_name);
            servers_.erase(hash);
            s.insert(hash);
        }
        for(uint32_t h: hash_circle_) {
            if(!s.count(h)) {
                tmp.push_back(h);
            }
        }
        hash_circle_.swap(tmp);
    }

    std::string ConsistentHashBalancer::GetEndpoint(const std::string &key) {
        std::lock_guard<std::mutex> lk(mu_);
        uint32_t hash = std::hash<std::string>()(key);
        auto it = std::lower_bound(hash_circle_.begin(), hash_circle_.end(), hash);
        if(it == hash_circle_.end()) {
            it = hash_circle_.begin();
        }
        return servers_[*it];
    }

}