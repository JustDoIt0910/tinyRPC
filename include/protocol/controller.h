//
// Created by just do it on 2023/12/4.
//

#ifndef TINYRPC_CONTROLLER_H
#define TINYRPC_CONTROLLER_H
#include <google/protobuf/service.h>
#include <optional>

namespace tinyRPC {
    class Controller: public google::protobuf::RpcController {
    public:

        // Client-side methods ---------------------------------------------
        // These calls may be made from the client side only.  Their results
        // are undefined on the server side (may crash).

        void Reset() override;

        bool Failed() const override;

        std::string ErrorText() const override;

        void StartCancel() override;

        void SetRequestId(const std::string& id);

        std::string RequestId();

        // Server-side methods ---------------------------------------------
        // These calls may be made from the server side only.  Their results
        // are undefined on the client side (may crash).

        void SetFailed(const std::string& reason) override;

        bool IsCanceled() const override;

        void NotifyOnCancel(google::protobuf::Closure* callback) override;

    private:
        std::optional<std::string> req_id_{std::nullopt};
    };
}

#endif //TINYRPC_CONTROLLER_H
