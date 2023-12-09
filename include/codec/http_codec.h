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
        enum Method { GET, POST, PUT, DELETE } method_;
        std::string url_;
        std::unordered_map<std::string, std::string> headers_;
    };

    class HttpRpcCodec: public Codec {
    public:
        DecodeResult Next(RpcMessage& message) override;

        std::string Encode(const RpcMessage& message) override;

    private:
        bool NextLine(std::string& line);
        DecodeResult DecodeRequestLine(RpcMessage& message);

        DecodeResult (HttpRpcCodec::*state_fn_)(RpcMessage&){nullptr};
        size_t prev_crlf_searched_{0};
    };

}

#endif //TINYRPC_HTTP_CODEC_H
