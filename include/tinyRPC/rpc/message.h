//
// Created by just do it on 2023/12/4.
//

#ifndef TINYRPC_MESSAGE_H
#define TINYRPC_MESSAGE_H
#include <string>
#include <cstring>
#include "tinyRPC/rpc/error.h"

namespace tinyRPC {

    struct RpcMessage {
        enum class MessageType {
            RPC_REQUEST, RPC_RESPONSE
        } type_;
        std::string msg_id_;
        std::string data_;

        explicit RpcMessage(MessageType type): type_(type) {}

        RpcMessage& operator=(RpcMessage&&) = default;

        RpcMessage(RpcMessage&&) = default;

        RpcMessage& operator=(const RpcMessage&) = default;

        RpcMessage(const RpcMessage&) = default;

        RpcMessage(MessageType type, std::string msg_id, std::string data)
                :type_(type), msg_id_(std::move(msg_id)), data_(std::move(data)) {}

        virtual uint32_t CRC() const;

        bool CheckCRC(uint32_t crc) const;

        virtual ~RpcMessage() noexcept = default;
    };

    struct RpcRequest: public RpcMessage {
        std::string full_method_name_;

        RpcRequest(): RpcMessage(MessageType::RPC_REQUEST) {};

        RpcRequest(std::string msg_id, std::string method_name, std::string data)
                :RpcMessage(MessageType::RPC_REQUEST, std::move(msg_id), std::move(data)),
                 full_method_name_(std::move(method_name)) {}

        uint32_t CRC() const override;
    };

    struct RpcResponse: public RpcMessage {
        uint8_t ec_;
        std::string error_detail_;

        RpcResponse(): RpcMessage(MessageType::RPC_RESPONSE), ec_() {};

        RpcResponse(std::string msg_id, uint8_t ec, std::string error_detail, std::string data)
                :RpcMessage(MessageType::RPC_RESPONSE, std::move(msg_id), std::move(data)),
                 ec_(ec), error_detail_(std::move(error_detail)) {}

        uint32_t CRC() const override;
    };

}

#endif //TINYRPC_MESSAGE_H
