//
// Created by just do it on 2023/12/9.
//
#include "codec/http_codec.h"

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
        line.erase(0, line.find_first_not_of(' '));
        line.erase(line.find_last_not_of(' ') + 1);
        std::string method = line.substr(0, line.find_first_of(' '));
        std::for_each(method.begin(), method.end(), [] (char& c) {
            c = static_cast<char>(std::tolower(c));
        });
        if(method == "get") { http_request_->method_ = HttpRequest::GET; }
        else if(method == "post") { http_request_->method_ = HttpRequest::POST; }
        else if(method == "put") { http_request_->method_ = HttpRequest::PUT; }
        else if(method == "delete") { http_request_->method_ = HttpRequest::DELETE; }
        else {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_METHOD_NOT_ALLOWED, method);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FAILED, message);
        }
        line.erase(0, line.find_first_of(' '));
        http_request_->url_ = line.substr(line.find_first_not_of(' '));
        return DecodeHeaders(message);
    }

    Codec::DecodeResult HttpRpcCodec::DecodeHeaders(RpcMessage& message) {
        std::string line;
        if(!NextLine(line)) SAVE_STATE(DecodeRequestLine)
        if(line.empty()) {
            if(http_request_->method_ != HttpRequest::GET) { return DecodeBody(message); }
            return Done(Codec::DecodeResult::SUCCESS, message);
        }

    }

    Codec::DecodeResult HttpRpcCodec::Done(Codec::DecodeResult res, RpcMessage& message) {
        if(res == Codec::DecodeResult::SUCCESS) {

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