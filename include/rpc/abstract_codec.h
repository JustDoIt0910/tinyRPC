//
// Created by just do it on 2023/12/5.
//

#ifndef TINYRPC_ABSTRACT_CODEC_H
#define TINYRPC_ABSTRACT_CODEC_H
#include "comm/endian.h"
#include "message.h"
#include "asio.hpp"

using namespace asio;

namespace tinyRPC {

    class Codec {
    public:

        const static size_t MaxReadBytes = 65536;

        enum class DecodeResult {SUCCESS, DECODING, FAILED};

        Codec(): read_index_(0), write_index_(0) {}

        mutable_buffers_1 Buffer() { return buffer(tmp_, MaxReadBytes); }

        void Consume(size_t length);

        virtual DecodeResult Next(RpcMessage& message) = 0;

        virtual std::string Encode(const RpcMessage& message) = 0;

        virtual ~Codec() = default;

    protected:
        size_t Readable() const { return write_index_ - read_index_; }

        size_t Writable() const { return buffer_.size() - write_index_; }

        void Discard(size_t length) {
            size_t len = std::min(length, Readable());
            read_index_ += len;
            if(read_index_ == write_index_) {
                read_index_ = write_index_ = 0;
            }
        }

        template<typename T=char*>
        T Fetch() {
            T val = *reinterpret_cast<T*>(Fetch());
            return NetworkToHost(val);
        }

        template<typename T>
        std::enable_if_t<(!std::is_same_v<T, std::string>) && (sizeof(T) > 1)>
        EncodeToBuffer(std::string& buffer, const T& val) {
            T val_be = HostToNetwork(val);
            buffer.append(reinterpret_cast<char*>(&val_be), sizeof(T));
        }

        template<typename T>
        std::enable_if_t<(!std::is_same_v<T, std::string>) && (sizeof(T) == 1)>
        EncodeToBuffer(std::string& buffer, const T& val) { buffer.push_back(val); }

        template<typename T>
        std::enable_if_t<std::is_same_v<T, std::string>>
        EncodeToBuffer(std::string& buffer, const T& val) { buffer.append(val); }

    private:
        char tmp_[MaxReadBytes] = {0};
        std::vector<char> buffer_;
        size_t read_index_;
        size_t write_index_;
    };

    template<>
    inline char* Codec::Fetch<char*>() { return static_cast<char*>(&buffer_[0]) + read_index_; }
}

#endif //TINYRPC_ABSTRACT_CODEC_H
