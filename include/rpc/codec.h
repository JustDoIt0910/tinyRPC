//
// Created by just do it on 2023/12/2.
//

#ifndef TINYRPC_CODEC_H
#define TINYRPC_CODEC_H
#include "abstract_codec.h"

namespace tinyRPC {

    class ProtobufRpcCodec: public Codec {
    public:
        using total_len_t = uint32_t;
        using message_id_len_t = uint16_t;
        using method_name_len_t = uint8_t;
        using crc_t = uint32_t;

        DecodeResult Next(RpcMessage& message) override;

        std::string Encode(const RpcMessage& request) override;

    private:
        template<typename MessageType>
        MessageType* GetMessage() {
            if(!message_) {
                message_ = std::make_unique<MessageType>();
            }
            return dynamic_cast<MessageType*>(message_.get());
        }

        void ResetMessage() { message_.reset(); }

        enum class DecodeState {
            DECODE_TOTAL_LEN, DECODE_MSG_ID, DECODE_METHOD, DECODE_DATA, DECODE_CRC
        } state_;
        uint32_t data_len_;
        uint32_t cur_field_len_;
        std::unique_ptr<RpcMessage> message_;
    };
}

#endif //TINYRPC_CODEC_H
