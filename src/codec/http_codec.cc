//
// Created by just do it on 2023/12/9.
//
#include "tinyRPC/codec/http_codec.h"
#include "tinyRPC/comm/string_util.h"
#include <sstream>
#include <iostream>
#include <cassert>

namespace tinyRPC {

    Codec::DecodeResult HttpRpcCodec::Next(RpcMessage &message) {
        if(!state_fn_) {
            http_request_ = std::make_unique<HttpRequest>();
            state_fn_ = &HttpRpcCodec::DecodeRequestLine;
        }
        return (this->*state_fn_)(message);
    }

    std::string HttpRpcCodec::Encode(const RpcMessage &message) {
        std::string resp;
        auto response = dynamic_cast<const RpcResponse&>(message);
        if(response.ec_ == rpc_error::error_code::RPC_SUCCESS) {
            std::stringstream ss;
            ss << "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
            << response.data_.length() << "\r\n";
            if(!response.msg_id_.empty()) {
                ss << "Query-Id: " << response.msg_id_ << "\r\n";
            }
            ss << "\r\n" << response.data_;
            resp = ss.str();
        }
        else {
            rpc_error::error_code ec(static_cast<rpc_error::error_code::error>(response.ec_), response.error_detail_);
            resp = MakeErrorResponse(500, "Internal Error", ec, response.msg_id_);
        }
        return resp;
    }

    std::string HttpRpcCodec::GetErrorResponse(const std::string& query_id) {
        auto internal_error = GetError();
        auto it = http_status_mapping_.find(internal_error->code());
        assert(it != http_status_mapping_.end());
        auto http_status = it->second;
        return MakeErrorResponse(http_status.first, http_status.second,
                                 *internal_error, query_id);
    }

    std::string HttpRpcCodec::MakeErrorResponse(int http_code, const std::string& reason,
                                                const rpc_error::error_code &internal_error,
                                                const std::string& query_id) {
        std::stringstream ss, err_ss;
        err_ss << "{\"error code\": " << internal_error.code()
        << ",\"message\": " << internal_error.message() << "}";
        std::string err_resp = err_ss.str();
        ss << "HTTP/1.1 " << http_code << " " << reason
        << "\r\nContent-Type: application/json\r\nContent-Length: "
           << err_resp.length() << "\r\n";
        if(!query_id.empty()) {
            ss << "Query-Id: " << query_id << "\r\n";
        }
        ss << "\r\n" << err_resp;
        return ss.str();
    }

    Codec::DecodeResult HttpRpcCodec::DecodeRequestLine(RpcMessage &message) {
        std::string line;
        if(!NextLine(line)) SAVE_HTTP_CODEC_STATE(DecodeRequestLine)
        StringUtil::Trim(line);
        http_request_->method_ = line.substr(0, line.find(' '));
        StringUtil::ToLower(http_request_->method_ );
        if(http_request_->method_ != "get" && http_request_->method_ != "post" &&
        http_request_->method_ != "put" && http_request_->method_ != "delete") {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_METHOD_NOT_ALLOWED, http_request_->method_);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FATAL, message);
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
            return Done(Codec::DecodeResult::FATAL, message);
        }
        return DecodeHeaders(message);
    }

    Codec::DecodeResult HttpRpcCodec::DecodeHeaders(RpcMessage& message) {
        while(true) {
            std::string line;
            if(!NextLine(line)) SAVE_HTTP_CODEC_STATE(DecodeRequestLine)
            if(line.empty()) { break; }
            std::string::size_type pos = line.find(':');
            if(pos == std::string::npos) {
                rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_BAD_REQUEST);
                SetError(std::move(ec));
                return Done(Codec::DecodeResult::FATAL, message);
            }
            std::string k = line.substr(0, pos);
            std::string v = line.substr(pos + 1);
            StringUtil::Trim(k);
            StringUtil::ToLower(k);
            StringUtil::Trim(v);
            http_request_->headers_[k] = v;
        }
        auto it = http_request_->headers_.find("content-type");
        if(it == http_request_->headers_.end()) {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_BAD_REQUEST);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FATAL, message);
        }
        if(it->second != "application/json") {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_UNSUPPORTED_MEDIA_TYPE,
                                     it->second);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FATAL, message);
        }
        it = http_request_->headers_.find("content-length");
        if(it == http_request_->headers_.end()) {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_LENGTH_REQUIRED);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FATAL, message);
        }
        try {
            content_len_ = std::stoi(it->second);
        }
        catch (std::invalid_argument& e) {
            rpc_error::error_code ec(rpc_error::error_code::RPC_HTTP_BAD_REQUEST);
            SetError(std::move(ec));
            return Done(Codec::DecodeResult::FATAL, message);
        }
        return DecodeBody(message);
    }

    Codec::DecodeResult HttpRpcCodec::DecodeBody(RpcMessage& message) {
        if(Readable() < content_len_) SAVE_HTTP_CODEC_STATE(DecodeBody)
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

    std::unordered_map<rpc_error::error_code::error, std::pair<int, std::string>>
    HttpRpcCodec::http_status_mapping_ = {
            {rpc_error::error_code::RPC_HTTP_METHOD_NOT_ALLOWED, {405, "Method Not Allowed"}},
            {rpc_error::error_code::RPC_HTTP_BAD_REQUEST, {400, "Bad Request"}},
            {rpc_error::error_code::RPC_HTTP_LENGTH_REQUIRED, {411, "Length Required"}},
            {rpc_error::error_code::RPC_HTTP_UNSUPPORTED_MEDIA_TYPE, {415, "Unsupported Media Type"}},
            {rpc_error::error_code::RPC_HTTP_VERSION_NOT_SUPPORTED, {505, "HTTP Version Not Supported"}},
    };

}