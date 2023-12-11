//
// Created by just do it on 2023/12/4.
//

#ifndef TINYRPC_ENDIAN_H
#define TINYRPC_ENDIAN_H
#include <endian.h>
#include <cstdint>

namespace tinyRPC {

    inline uint8_t HostToNetwork(const uint8_t& val) { return val; }

    inline uint16_t HostToNetwork(const uint16_t& val) { return htobe16(val); }

    inline uint32_t HostToNetwork(const uint32_t& val) { return htobe32(val); }

    inline uint64_t HostToNetwork(const uint64_t& val) { return htobe64(val); }

    inline uint8_t NetworkToHost(const uint8_t& val) { return val; }

    inline uint16_t NetworkToHost(const uint16_t& val) { return be16toh(val); }

    inline uint32_t NetworkToHost(const uint32_t& val) { return be32toh(val); }

    inline uint64_t NetworkToHost(const uint64_t& val) { return be64toh(val); }

}

#endif //TINYRPC_ENDIAN_H
