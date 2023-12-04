//
// Created by just do it on 2023/12/4.
//
#include "rpc/controller.h"
#include "comm/uuid.h"

namespace tinyRPC {

    void Controller::Reset() {}

    bool Controller::Failed() const {}

    std::string Controller::ErrorText() const {}

    void Controller::StartCancel() {}

    void Controller::SetRequestId(const std::string& id) { req_id_ = id; }

    std::string Controller::RequestId() {
        if(!req_id_.has_value()) {
            req_id_ = UUID::generate().to_string();
        }
        return req_id_.value();
    }

    void Controller::SetFailed(const std::string& reason) {}

    bool Controller::IsCanceled() const {}

    void Controller::NotifyOnCancel(google::protobuf::Closure* callback) {}

}