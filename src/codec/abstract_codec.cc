//
// Created by just do it on 2023/12/5.
//
#include "tinyRPC/codec/abstract_codec.h"
#include "asio.hpp"

namespace tinyRPC {

    asio::mutable_buffers_1 Codec::Buffer() { return asio::buffer(tmp_, MaxReadBytes); }

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

    char Codec::CRLF_[] = "\r\n";
}