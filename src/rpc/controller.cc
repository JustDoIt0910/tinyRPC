//
// Created by just do it on 2023/12/4.
//
#include "rpc/controller.h"
#include "comm/uuid.h"

namespace tinyRPC {

    void Controller::Reset() {}

    bool Controller::Failed() const { return false; }

    std::string Controller::ErrorText() const { return ""; }

    void Controller::StartCancel() {}

    void Controller::SetRequestId(const std::string& id) { req_id_ = id; }

    std::string Controller::RequestId() {
        if(!req_id_) {
            req_id_ = UUID::generate().to_string();
        }
        return *req_id_;
    }

    void Controller::SetTimeout(int milliseconds) { connect_timeout_ = milliseconds; }

    std::optional<int> Controller::Timeout() { return connect_timeout_; }

    void Controller::SetAsync(bool set) { async_ = set; }

    bool Controller::Async() const { return async_; }

    void Controller::SetFailed(const std::string& reason) {}

    bool Controller::IsCanceled() const {}

    void Controller::NotifyOnCancel(google::protobuf::Closure* callback) {}

}