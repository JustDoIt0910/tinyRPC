//
// Created by just do it on 2023/12/2.
//

#ifndef TINYRPC_CODEC_H
#define TINYRPC_CODEC_H
#include "abstract_codec.h"

namespace tinyRPC {

    class ProtobufRpcCodec: public Codec {
    public:

        // field types in the rpc request packet
        using total_len_t = uint32_t;
        using message_id_len_t = uint16_t;
        using method_name_len_t = uint8_t;
        using crc_t = uint32_t;

        const static size_t RequestFixedSize = sizeof(total_len_t) + sizeof(message_id_len_t) +
                sizeof(method_name_len_t) + sizeof(crc_t);

        // field types in the rpc response packet
        using error_code_t = uint8_t;
        using error_len_t = uint8_t;

        const static size_t ResponseFixedSize = sizeof(total_len_t) + sizeof(message_id_len_t) +
                sizeof(error_code_t) + sizeof(error_len_t) + sizeof(crc_t);

        DecodeResult Next(RpcMessage& message) override;

        std::string Encode(const RpcMessage& message) override;

    private:
        void NewMessage(RpcMessage::MessageType type) {
            if(type == RpcMessage::MessageType::RPC_REQUEST) {
                message_ = std::make_unique<RpcRequest>();
            }
            else { message_ = std::make_unique<RpcResponse>(); }
        }

        template<typename MessageType>
        MessageType* GetMessage() { return dynamic_cast<MessageType*>(message_.get()); }

        void ResetMessage() { message_.reset(); }

        DecodeResult DecodeTotalLen(RpcMessage&);
        DecodeResult DecodeMsgId(RpcMessage&);
        DecodeResult DecodeData(RpcMessage&);
        DecodeResult DecodeCrc(RpcMessage&);
        DecodeResult DecodeMethod(RpcMessage&);
        DecodeResult DecodeErrorCode(RpcMessage&);
        DecodeResult DecodeError(RpcMessage&);
        DecodeResult Done(DecodeResult res, RpcMessage&);

        DecodeResult (ProtobufRpcCodec::*state_fn_)(RpcMessage&){nullptr};
        size_t fixed_len_;
        uint32_t data_len_;
        uint32_t cur_field_len_;
        std::unique_ptr<RpcMessage> message_;
    };
}

#endif //TINYRPC_CODEC_H
