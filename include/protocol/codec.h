//
// Created by just do it on 2023/12/2.
//

#ifndef TINYRPC_CODEC_H
#define TINYRPC_CODEC_H
#include <cstdint>
#include <string>
#include <vector>
#include <endian.h>
#include "asio.hpp"
#include "error.h"

using namespace asio;

namespace tinyRPC {

    struct RpcMessage {
        enum class MessageType {
            RPC_REQUEST, RPC_RESPONSE
        } type_;
        std::string msg_id_;
        std::string data_;

        explicit RpcMessage(MessageType type): type_(type) {}

        RpcMessage(MessageType type, std::string msg_id, std::string data)
        :type_(type), msg_id_(std::move(msg_id)), data_(std::move(data)) {}
    };

    struct RpcRequest: public RpcMessage {
        std::string full_method_name_;

        RpcRequest(): RpcMessage(MessageType::RPC_REQUEST) {};

        RpcRequest(std::string msg_id, std::string method_name, std::string data)
        :RpcMessage(MessageType::RPC_REQUEST, std::move(msg_id), std::move(data)),
        full_method_name_(std::move(method_name)) {}
    };

    struct RpcResponse: public RpcMessage {
        ErrorCode ec_;
        std::string error_info_;

        RpcResponse(): RpcMessage(MessageType::RPC_RESPONSE), ec_() {};

        RpcResponse(std::string msg_id, ErrorCode ec, std::string error_info, std::string data)
        :RpcMessage(MessageType::RPC_RESPONSE, std::move(msg_id), std::move(data)),
        ec_(ec), error_info_(std::move(error_info)) {}
    };

    class Codec {
    public:
        const static size_t kMaxReadBytes = 1024 * 1024;

        Codec(): read_index_(0), write_index_(0) {}

        mutable_buffers_1 Buffer() { return buffer(tmp_, kMaxReadBytes); }

        void Consume(size_t length);

        virtual bool Next(RpcMessage& message) = 0;

        virtual ~Codec() = default;

    protected:
        size_t Readable() const { return write_index_ - read_index_; }

        size_t Writable() const { return buffer_.size() - write_index_; }

        char* Peek() { return static_cast<char*>(&buffer_[0]) + read_index_; }

        void Discard(size_t length) {
            size_t len = std::min(length, Readable());
            read_index_ += len;
            if(read_index_ == write_index_) {
                read_index_ = write_index_ = 0;
            }
        }

        uint16_t Peek16() {
            uint16_t val = *reinterpret_cast<uint16_t*>(Peek());
            return be16toh(val);
        }

        uint32_t Peek32() {
            uint32_t val = *reinterpret_cast<uint32_t*>(Peek());
            return be32toh(val);
        }

    private:
        char tmp_[kMaxReadBytes] = {0};
        std::vector<char> buffer_;
        size_t read_index_;
        size_t write_index_;
    };

    class ProtobufRpcCodec: public Codec {
    public:
        bool Next(RpcMessage& message) final;
    };

    class HttpRpcCodec: public Codec {
    public:
        bool Next(RpcMessage& message) final;
    };
}

#endif //TINYRPC_CODEC_H
