//
// Created by just do it on 2023/12/9.
//

#ifndef TINYRPC_HTTP_CODEC_H
#define TINYRPC_HTTP_CODEC_H
#include "abstract_codec.h"

#define SAVE_STATE(state) { state_fn_ = &HttpRpcCodec::state; \
                            return DecodeResult::DECODING; }

namespace tinyRPC {

    struct HttpRequest {
        std::string method_;
        std::string url_;
        std::unordered_map<std::string, std::string> headers_;
        std::string body_;

        void ToInternalMessage(RpcMessage& message) {
            auto& rpc_request = dynamic_cast<RpcRequest&>(message);
            auto it = headers_.find("query-id");
            if(it != headers_.end()) { rpc_request.msg_id_ = std::move(it->second); }
            rpc_request.full_method_name_.append(method_);
            rpc_request.full_method_name_.push_back(' ');
            rpc_request.full_method_name_.append(url_);
            rpc_request.data_ = std::move(body_);
        }
    };

    class HttpRpcCodec: public Codec {
    public:
        DecodeResult Next(RpcMessage& message) override;

        std::string Encode(const RpcMessage& message) override;

    private:
        bool NextLine(std::string& line);
        DecodeResult DecodeRequestLine(RpcMessage&);
        DecodeResult DecodeHeaders(RpcMessage&);
        DecodeResult DecodeBody(RpcMessage&);
        DecodeResult Done(DecodeResult res, RpcMessage&);

        DecodeResult (HttpRpcCodec::*state_fn_)(RpcMessage&){nullptr};
        uint16_t prev_crlf_searched_{0};
        uint16_t content_len_;
        std::unique_ptr<HttpRequest> http_request_;
    };

}

#endif //TINYRPC_HTTP_CODEC_H
