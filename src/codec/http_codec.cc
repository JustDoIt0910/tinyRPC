//
// Created by just do it on 2023/12/9.
//
#include "codec/http_codec.h"

namespace tinyRPC {

    Codec::DecodeResult HttpRpcCodec::Next(RpcMessage &message) {

    }

    std::string HttpRpcCodec::Encode(const RpcMessage &message) {

    }

    bool HttpRpcCodec::NextLine(std::string& line) {
        const char* begin = Fetch();
        const char* crlf = FindCRLF(begin + prev_crlf_searched_, &prev_crlf_searched_);
        if(!crlf) { return false; }
        line = std::string(begin, crlf - begin);
        Discard(crlf - begin + 2);
        prev_crlf_searched_ = 0;
        return true;
    }

}