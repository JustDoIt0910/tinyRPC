//
// Created by just do it on 2023/12/5.
//
#include "rpc/abstract_router.h"
#include <google/protobuf/descriptor.h>

namespace tinyRPC {

    void Router::RegisterService(tinyRPC::ServicePtr service) {
        std::string name = service->GetDescriptor()->full_name();
        if(service_map_.find(name) != service_map_.end()) {
            // TODO use log library
            std::cout << "Service " << name << " has already registered" << std::endl;
            return;
        }
        service_map_[name] = service;
    }

    bool Router::ParseServiceMethod(const std::string &full_name, std::string& service, std::string& method) const {
        size_t dot = full_name.find('.');
        if(dot == std::string::npos) { return false; }
        service = full_name.substr(0, dot);
        method = full_name.substr(dot + 1);
        return true;
    }

    ServicePtr Router::GetService(const std::string &service) const {
        auto it = service_map_.find(service);
        if(it == service_map_.end()) { return ServicePtr{}; }
        return it->second;
    }

}