//
// Created by just do it on 2023/12/4.
//

#ifndef TINYRPC_CLOSURE_H
#define TINYRPC_CLOSURE_H
#include <google/protobuf/stubs/callback.h>
#include <functional>

namespace tinyRPC {
    class RpcClosure: public google::protobuf::Closure {
    public:
        explicit RpcClosure(std::function<void()> cb) : cb_(cb) {}

        ~RpcClosure() = default;

        void Run() {
            if(cb_) { cb_(); }
        }

    private:
        std::function<void()> cb_ {nullptr};
    };
}

#endif //TINYRPC_CLOSURE_H
