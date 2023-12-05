//
// Created by just do it on 2023/12/4.
//

#ifndef TINYRPC_CLOSURE_H
#define TINYRPC_CLOSURE_H
#include <google/protobuf/stubs/callback.h>
#include <functional>
#include <utility>

namespace tinyRPC {
    class RpcClosure: public google::protobuf::Closure {
    public:
        explicit RpcClosure(std::function<void()> cb) : cb_(std::move(cb)) {}

        ~RpcClosure() override = default;

        void Run() override {
            if(cb_) { cb_(); }
        }

    private:
        std::function<void()> cb_ {nullptr};
    };
}

#endif //TINYRPC_CLOSURE_H
