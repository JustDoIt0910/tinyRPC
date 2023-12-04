//
// Created by just do it on 2023/12/2.
//
#include "rpc/codec.h"

namespace tinyRPC {

    Codec::DecodeResult ProtobufRpcCodec::Next(RpcMessage &message) {
        Codec::DecodeResult res = Codec::DecodeResult::DECODING;
        if(message.type_ == RpcMessage::MessageType::RPC_REQUEST) {
            switch (state_) {
                case DecodeState::DECODE_TOTAL_LEN:
                    if(Readable() < sizeof(total_len_t)) break;
                    if(Fetch<total_len_t>() <= 11) {
                        res = Codec::DecodeResult::FAILED;
                        break;
                    }
                    data_len_ = Fetch<total_len_t>() - sizeof(total_len_t) - sizeof(message_id_len_t) -
                            sizeof(method_name_len_t) - sizeof(crc_t);
                    Discard(sizeof(total_len_t));
                    state_ = DecodeState::DECODE_MSG_ID;
                case DecodeState::DECODE_MSG_ID:
                    if(cur_field_len_ == 0) {
                        if(Readable() < sizeof(message_id_len_t)) break;
                        cur_field_len_ = Fetch<message_id_len_t>();
                        Discard(sizeof(message_id_len_t));
                    }
                    if(Readable() < cur_field_len_) break;
                    GetMessage<RpcRequest>()->msg_id_ = std::string(Fetch(), cur_field_len_);
                    Discard(cur_field_len_);
                    data_len_ -= cur_field_len_;
                    cur_field_len_ = 0;
                    state_ = DecodeState::DECODE_METHOD;
                case DecodeState::DECODE_METHOD:
                    if(cur_field_len_ == 0) {
                        if(Readable() < sizeof(method_name_len_t)) break;
                        cur_field_len_ = Fetch<method_name_len_t>();
                        Discard(sizeof(method_name_len_t));
                    }
                    if(Readable() < cur_field_len_) break;
                    GetMessage<RpcRequest>()->full_method_name_ = std::string(Fetch(), cur_field_len_);
                    Discard(cur_field_len_);
                    data_len_ -= cur_field_len_;
                    cur_field_len_ = 0;
                    state_ = DecodeState::DECODE_DATA;
                case DecodeState::DECODE_DATA:
                    if(Readable() < data_len_) break;
                    GetMessage<RpcRequest>()->data_ = std::string(Fetch(), data_len_);
                    Discard(data_len_);
                    state_ = DecodeState::DECODE_CRC;
                case DecodeState::DECODE_CRC:
                    if(Readable() < sizeof(crc_t)) break;
                    if(GetMessage<RpcRequest>()->CheckCRC(Fetch<crc_t>())) {
                        auto& new_request = dynamic_cast<RpcRequest&>(message);
                        new_request = std::move(*GetMessage<RpcRequest>());
                        res = Codec::DecodeResult::SUCCESS;
                    }
                    else { res = Codec::DecodeResult::FAILED; }
                    Discard(sizeof(crc_t));
                    ResetMessage();
                    state_ = DecodeState::DECODE_TOTAL_LEN;
            }
        }
        else {

        }
        return res;
    }

    std::string ProtobufRpcCodec::Encode(const RpcMessage& message) {
        std::string buffer;
        if(message.type_ == RpcMessage::MessageType::RPC_REQUEST) {
            auto& request = dynamic_cast<const RpcRequest&>(message);
            total_len_t total_len = sizeof(total_len_t) + sizeof(message_id_len_t) +
                                    sizeof(method_name_len_t) + sizeof(crc_t);
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

            EncodeToBuffer(buffer, request.CRC());
        }
        else {
            auto& response = dynamic_cast<const RpcRequest&>(message);
        }
        return buffer;
    }
}