//
// Created by just do it on 2023/12/4.
//

#ifndef TINYRPC_CLOSURE_H
#define TINYRPC_CLOSURE_H
#include <google/protobuf/stubs/callback.h>
#include <google/protobuf/message.h>
#include "rpc/error.h"
#include <functional>
#include <utility>

namespace tinyRPC {

    using Handler = std::function<void(rpc_error::error_code, google::protobuf::Message*)>;

    class RpcClosure: public google::protobuf::Closure {
    public:
        explicit RpcClosure(Handler cb,
                            const google::protobuf::Descriptor* response_message_descriptor):
        response_type_desc_(response_message_descriptor) {
            cb_ = [this, cb = std::move(cb)] () {
                cb(ec_, response_);
            };
        }

        void SetErrorCode(rpc_error::error_code ec) { ec_ = std::move(ec); }

        google::protobuf::Message* GetResponse() {
            if(!response_) {
                auto prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(response_type_desc_);
                response_ = prototype->New();
            }
            return response_;
        }

        ~RpcClosure() override = default;

        void Run() override {
            if(cb_) { cb_(); }
        }

    private:
        std::function<void()> cb_ {nullptr};
        google::protobuf::Message* response_{nullptr};
        const google::protobuf::Descriptor* response_type_desc_;
        rpc_error::error_code ec_;
    };

    template<typename ResponseMessage>
    inline RpcClosure* NewRpcClosure(Handler cb) {
        return new RpcClosure(cb, ResponseMessage::descriptor());
    }
}

#endif //TINYRPC_CLOSURE_H
