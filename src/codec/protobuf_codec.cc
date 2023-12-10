//
// Created by just do it on 2023/12/2.
//
#include "codec/protobuf_codec.h"

namespace tinyRPC {

    Codec::DecodeResult ProtobufRpcCodec::Next(RpcMessage &message) {
        if(!state_fn_) {
            NewMessage(message.type_);
            fixed_len_ = (message.type_ == RpcMessage::MessageType::RPC_REQUEST) ?
                    RequestFixedSize : ResponseFixedSize;
            state_fn_ = &ProtobufRpcCodec::DecodeTotalLen;
        }
        return (this->*state_fn_)(message);
    }

    std::string ProtobufRpcCodec::Encode(const RpcMessage& message) {
        std::string buffer;
        if(message.type_ == RpcMessage::MessageType::RPC_REQUEST) {
            auto& request = dynamic_cast<const RpcRequest&>(message);
            total_len_t total_len = RequestFixedSize;
            total_len += request.msg_id_.length();
            total_len += request.full_method_name_.length();
            total_len += request.data_.length();
            EncodeToBuffer(buffer, total_len);

            message_id_len_t msg_id_len = request.msg_id_.length();
            EncodeToBuffer(buffer, msg_id_len);
            EncodeToBuffer(buffer, request.msg_id_);

            method_name_len_t method_name_len = request.full_method_name_.length();
            EncodeToBuffer(buffer, method_name_len);
            EncodeToBuffer(buffer, request.full_method_name_);
            EncodeToBuffer(buffer, request.data_);
            //EncodeToBuffer(buffer, request.CRC());
            uint32_t crc = 0;
            EncodeToBuffer(buffer, crc);
        }
        else {
            auto& response = dynamic_cast<const RpcResponse&>(message);
            total_len_t total_len = ResponseFixedSize;
            total_len += response.msg_id_.length();
            total_len += response.error_detail_.length();
            total_len += response.data_.length();
            EncodeToBuffer(buffer, total_len);

            message_id_len_t msg_id_len = response.msg_id_.length();
            EncodeToBuffer(buffer, msg_id_len);
            EncodeToBuffer(buffer, response.msg_id_);

            error_code_t code = response.ec_;
            EncodeToBuffer(buffer, code);

            error_len_t error_len = response.error_detail_.length();
            EncodeToBuffer(buffer, error_len);
            EncodeToBuffer(buffer, response.error_detail_);
            EncodeToBuffer(buffer, response.data_);
            //EncodeToBuffer(buffer, response.CRC());
            uint32_t crc = 0;
            EncodeToBuffer(buffer, crc);
        }
        return buffer;
    }

    Codec::DecodeResult ProtobufRpcCodec::DecodeTotalLen(RpcMessage& message) {
        if(Readable() < sizeof(total_len_t)) SAVE_STATE(DecodeTotalLen)
        auto total = Fetch<total_len_t>();
        Discard(sizeof(total_len_t));
        if(total <= fixed_len_) {
            return Done(Codec::DecodeResult::FATAL, message);
        }
        data_len_ = total - fixed_len_;
        return DecodeMsgId(message);
    }

    Codec::DecodeResult ProtobufRpcCodec::DecodeMsgId(RpcMessage& message) {
        if(cur_field_len_ == 0) {
            if(Readable() < sizeof(message_id_len_t)) SAVE_STATE(DecodeMsgId)
            cur_field_len_ = Fetch<message_id_len_t>();
            Discard(sizeof(message_id_len_t));
        }
        if(Readable() < cur_field_len_) SAVE_STATE(DecodeMsgId)
        GetMessage<RpcMessage>()->msg_id_ = std::string(Fetch(), cur_field_len_);
        Discard(cur_field_len_);
        data_len_ -= cur_field_len_;
        cur_field_len_ = 0;
        if(message_->type_ == RpcMessage::MessageType::RPC_REQUEST) {
            return DecodeMethod(message);
        }
        else { return DecodeErrorCode(message); }
    }

    Codec::DecodeResult ProtobufRpcCodec::DecodeData(RpcMessage& message) {
        if(Readable() < data_len_) SAVE_STATE(DecodeData);
        GetMessage<RpcMessage>()->data_ = std::string(Fetch(), data_len_);
        Discard(data_len_);
        return DecodeCrc(message);
    }

    Codec::DecodeResult ProtobufRpcCodec::DecodeCrc(RpcMessage& message) {
        if(Readable() < sizeof(crc_t)) SAVE_STATE(DecodeCrc)
        DecodeResult res = Codec::DecodeResult::SUCCESS;
//        if(!GetMessage<RpcMessage>()->CheckCRC(Fetch<crc_t>())) {
//            res = Codec::DecodeResult::ERROR;
//        }
        Discard(sizeof(crc_t));
        return Done(res, message);
    }

    Codec::DecodeResult ProtobufRpcCodec::DecodeMethod(RpcMessage& message) {
        if(cur_field_len_ == 0) {
            if(Readable() < sizeof(method_name_len_t)) SAVE_STATE(DecodeMethod)
            cur_field_len_ = Fetch<method_name_len_t>();
            Discard(sizeof(method_name_len_t));
        }
        if(Readable() < cur_field_len_) SAVE_STATE(DecodeMethod)
        GetMessage<RpcRequest>()->full_method_name_ = std::string(Fetch(), cur_field_len_);
        Discard(cur_field_len_);
        data_len_ -= cur_field_len_;
        cur_field_len_ = 0;
        return DecodeData(message);
    }

    Codec::DecodeResult ProtobufRpcCodec::DecodeErrorCode(RpcMessage& message) {
        if(Readable() < sizeof(error_code_t)) SAVE_STATE(DecodeErrorCode);
        GetMessage<RpcResponse>()->ec_ = Fetch<error_code_t>();
        Discard(sizeof(error_code_t));
        return DecodeError(message);
    }

    Codec::DecodeResult ProtobufRpcCodec::DecodeError(RpcMessage& message) {
        if(cur_field_len_ == 0) {
            if(Readable() < sizeof(error_len_t)) SAVE_STATE(DecodeError)
            cur_field_len_ = Fetch<error_len_t>();
            Discard(sizeof(error_len_t));
        }
        if(Readable() < cur_field_len_) SAVE_STATE(DecodeError)
        GetMessage<RpcResponse>()->error_detail_ = std::string(Fetch(), cur_field_len_);
        Discard(cur_field_len_);
        data_len_ -= cur_field_len_;
        cur_field_len_ = 0;
        return DecodeData(message);
    }

    Codec::DecodeResult ProtobufRpcCodec::Done(Codec::DecodeResult res, RpcMessage& message) {
        if(res == Codec::DecodeResult::SUCCESS) {
            if(message_->type_ == RpcMessage::MessageType::RPC_REQUEST) {
                auto& new_request = dynamic_cast<RpcRequest&>(message);
                new_request = std::move(*GetMessage<RpcRequest>());
            }
            else {
                auto& new_response = dynamic_cast<RpcResponse&>(message);
                new_response = std::move(*GetMessage<RpcResponse>());
            }
        }
        state_fn_ = nullptr;
        ResetMessage();
        return res;
    }

    std::string ProtobufRpcCodec::GetErrorResponse(const std::string &query_id) {
        return {};
    }

}