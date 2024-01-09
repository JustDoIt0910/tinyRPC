//
// Created by just do it on 2024/1/8.
//
#include "tinyRPC/client/lb.h"

namespace tinyRPC {

    void RoundRobinBalancer::AddEndpoint(const std::string &endpoint) {
        endpoints_.insert(endpoint);
        cur_ = endpoints_.begin();
    }

    void RoundRobinBalancer::RemoveEndpoint(const std::string &endpoint) {
        endpoints_.erase(endpoint);
        cur_ = endpoints_.begin();
    }

    std::string RoundRobinBalancer::GetEndpoint(const std::string &key) {
        if(cur_ == endpoints_.end()) {
            cur_ = endpoints_.begin();
        }
        if(cur_ == endpoints_.end()) { return {}; }
        return *(cur_++);
    }

}