//
// Created by just do it on 2023/12/5.
//

#ifndef TINYRPC_ABSTRACT_CODEC_H
#define TINYRPC_ABSTRACT_CODEC_H
#include "tinyRPC/comm/endian.h"
#include "tinyRPC/rpc/message.h"
#include <memory>
#include <algorithm>

namespace asio { class mutable_buffers_1; }

namespace tinyRPC {

    class Codec {
    public:

        const static size_t MaxReadBytes = 8192;

        enum class DecodeResult {SUCCESS, DECODING, ERROR, FATAL};

        Codec(): read_index_(0), write_index_(0) {}

        asio::mutable_buffers_1 Buffer();

        void Consume(size_t length);

        virtual DecodeResult Next(RpcMessage& message) = 0;

        virtual std::string Encode(const RpcMessage& message) = 0;

        virtual std::string GetErrorResponse(const std::string& query_id) = 0;

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

        template<typename T=const char*>
        T Fetch() const {
            const T val = *reinterpret_cast<const T*>(Fetch());
            return NetworkToHost(val);
        }

        const char* FindCRLF(const char* start, uint16_t* prev_searched) {
            const char* end = Begin() + write_index_;
            const char* pos = std::search(start, end, CRLF_, CRLF_ + 2);
            if(pos == end) {
                *prev_searched = (end - start) + *prev_searched;
                return nullptr;
            }
            return pos;
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

    protected:
        void SetError(rpc_error::error_code ec) {
            decode_error_ = std::make_unique<rpc_error::error_code>(std::move(ec));
        }

        std::unique_ptr<rpc_error::error_code> GetError() { return std::move(decode_error_); }

    private:
        const char* Begin() const { return &*buffer_.begin(); }

        char tmp_[MaxReadBytes] = {0};
        std::vector<char> buffer_;
        size_t read_index_;
        size_t write_index_;
        std::unique_ptr<rpc_error::error_code> decode_error_;

        static char CRLF_[];
    };

    template<>
    inline const char* Codec::Fetch<const char*>() const { return Begin() + read_index_; }
}

#endif //TINYRPC_ABSTRACT_CODEC_H
