//
// Created by just do it on 2023/12/2.
//
#include "protocol/codec.h"
#include "comm/crc32.h"
#include <cassert>

namespace tinyRPC {

    void Codec::Consume(size_t length) {
        size_t writable = Writable();
        if(writable >= length) {
            memcpy(&buffer_[0] + write_index_, tmp_, length);
            write_index_ += length;
            return;
        }
        if(read_index_ + writable >= length) {
            size_t readable = Readable();
            memmove(&buffer_[0], &buffer_[0] + read_index_, readable);
            read_index_ = 0;
            memcpy(&buffer_[0] + readable, tmp_, length);
            write_index_ = readable + length;
            return;
        }
        size_t need = length - writable;
        buffer_.resize(buffer_.size() + need);
        assert(Writable() >= length);
        memcpy(&buffer_[0] + write_index_, tmp_, length);
        write_index_ += length;
    }

    bool ProtobufRpcCodec::Next(RpcMessage &message) {
        if(message.type_ == RpcMessage::MessageType::RPC_REQUEST) {

        }
        else {

        }
        return false;
    }

    std::string ProtobufRpcCodec::Encode(const RpcMessage& message) {
        std::string buffer;
        if(message.type_ == RpcMessage::MessageType::RPC_REQUEST) {
            auto request = dynamic_cast<const RpcRequest&>(message);
            uint32_t total_len = 11;
            total_len += request.msg_id_.length();
            total_len += request.full_method_name_.length();
            total_len += request.data_.length();
            total_len = htobe32(total_len);
            buffer.append(reinterpret_cast<char*>(&total_len), sizeof(total_len));

            uint16_t msg_id_len = request.msg_id_.length();
            msg_id_len = htobe16(msg_id_len);
            buffer.append(reinterpret_cast<char*>(&msg_id_len), sizeof(msg_id_len));
            buffer.append(request.msg_id_);

            uint8_t method_name_len = request.full_method_name_.length();
            buffer.push_back(static_cast<char>(method_name_len));
            buffer.append(request.full_method_name_);
            buffer.append(request.data_);

            uint32_t crc = crc32(reinterpret_cast<uint8_t*>(buffer.data()), buffer.length());
            crc = htobe32(crc);
            buffer.append(reinterpret_cast<char*>(&crc), sizeof(crc));
        }
        else {
            auto response = dynamic_cast<const RpcRequest&>(message);
        }
        return buffer;
    }
}