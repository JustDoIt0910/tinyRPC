//
// Created by just do it on 2023/12/9.
//
#include "codec/http_codec.h"
#include "comm/string_util.h"
#include <iostream>

namespace tinyRPC {

    Codec::DecodeResult HttpRpcCodec::Next(RpcMessage &message) {
        if(!state_fn_) {
            http_request_ = std::make_unique<HttpRequest>();
            state_fn_ = &HttpRpcCodec::DecodeRequestLine;
        }
        return (this->*state_fn_)(message);
    }

    std::string HttpRpcCodec::Encode(const RpcMessage &message) {

    }

    Codec::DecodeResult HttpRpcCodec::DecodeRequestLine(RpcMessage &message) {
        std::string line;
        if(!NextLine(line)) SAVE_STATE(DecodeRequestLine)
        StringUtil::Trim(line);
        http_request_->method_ = line.substr(0, line.find(' '));
        StringUtil::ToLower(http_request_->method_ );
        if(http_request_->method_ != "get" && http_request_->method_ != "post" &&
        http_request_->method_ != "put" && http_request_->method_ != "delete") {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_METHOD_NOT_ALLOWED, http_request_->method_);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FAILED, message);
        }
        line.erase(0, line.find(' '));
        auto url_start = line.find_first_not_of(' ');
        auto url_end = line.find(' ', url_start);
        http_request_->url_ = line.substr(url_start, url_end - url_start);
        std::string version = line.substr(url_end + 1);
        StringUtil::Trim(version);
        if(version != "HTTP/1.1") {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_VERSION_NOT_SUPPORTED, version);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FAILED, message);
        }
        return DecodeHeaders(message);
    }

    Codec::DecodeResult HttpRpcCodec::DecodeHeaders(RpcMessage& message) {
        while(true) {
            std::string line;
            if(!NextLine(line)) SAVE_STATE(DecodeRequestLine)
            if(line.empty()) { break; }
            std::vector<std::string> kv;
            StringUtil::Split(line, ":", kv);
            if(kv.size() != 2) {
                rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_BAD_REQUEST);
                SetError(std::move(ec));
                return Done(Codec::DecodeResult::FAILED, message);
            }
            StringUtil::Trim(kv[0]);
            StringUtil::ToLower(kv[0]);
            StringUtil::Trim(kv[1]);
            http_request_->headers_[kv[0]] = kv[1];
        }
        auto it = http_request_->headers_.find("content-type");
        if(it == http_request_->headers_.end()) {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_BAD_REQUEST);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FAILED, message);
        }
        if(it->second != "application/json") {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_UNSUPPORTED_MEDIA_TYPE,
                                     it->second);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FAILED, message);
        }
        it = http_request_->headers_.find("content-length");
        if(it == http_request_->headers_.end()) {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_LENGTH_REQUIRED);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FAILED, message);
        }
        try {
            content_len_ = std::stoi(it->second);
        }
        catch (std::invalid_argument& e) {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_BAD_REQUEST);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FAILED, message);
        }
        return DecodeBody(message);
    }

    Codec::DecodeResult HttpRpcCodec::DecodeBody(RpcMessage& message) {
        if(Readable() < content_len_) SAVE_STATE(DecodeBody)
        http_request_->body_ = std::string(Fetch(), content_len_);
        Discard(content_len_);
        return Done(Codec::DecodeResult::SUCCESS, message);
    }

    Codec::DecodeResult HttpRpcCodec::Done(Codec::DecodeResult res, RpcMessage& message) {
        if(res == Codec::DecodeResult::SUCCESS) {
            http_request_->ToInternalMessage(message);
        }
        state_fn_ = nullptr;
        http_request_.reset();
        return res;
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