//
// Created by just do it on 2023/12/11.
//
#include "tinyRPC/rpc/message.h"
#include "CRCpp/inc/CRC.h"

namespace tinyRPC {

    uint32_t RpcMessage::CRC() const {
        uint32_t crc = CRC::Calculate(msg_id_.data(), msg_id_.length(), CRC::CRC_32());
        return CRC::Calculate(data_.data(), data_.length(), CRC::CRC_32(), crc);
    }

    bool RpcMessage::CheckCRC(uint32_t crc) const { return CRC() == crc; }

    uint32_t RpcRequest::CRC() const {
        uint32_t crc = RpcMessage::CRC();
        return CRC::Calculate(full_method_name_.data(), full_method_name_.length(), CRC::CRC_32(), crc);
    }

    uint32_t RpcResponse::CRC() const {
        uint32_t crc = RpcMessage::CRC();
        return CRC::Calculate(error_detail_.data(), error_detail_.length(), CRC::CRC_32(), crc);
    }

}