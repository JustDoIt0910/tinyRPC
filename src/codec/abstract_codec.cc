//
// Created by just do it on 2023/12/5.
//
#include "tinyRPC/codec/abstract_codec.h"
#include "asio.hpp"

namespace tinyRPC {

    Codec::Codec(): read_index_(0), write_index_(0) {}

    size_t Codec::Readable() const { return write_index_ - read_index_; }

    size_t Codec::Writable() const { return buffer_.size() - write_index_; }

    const char* Codec::Begin() const { return &*buffer_.begin(); }

    void Codec::Discard(size_t length) {
        size_t len = std::min(length, Readable());
        read_index_ += len;
        if(read_index_ == write_index_) {
            read_index_ = write_index_ = 0;
        }
    }

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

    const char* Codec::FindCRLF(const char* start, uint16_t* prev_searched) {
        const char* end = Begin() + write_index_;
        const char* pos = std::search(start, end, CRLF_, CRLF_ + 2);
        if(pos == end) {
            *prev_searched = (end - start) + *prev_searched;
            return nullptr;
        }
        return pos;
    }

    char Codec::CRLF_[] = "\r\n";
}