//
// Created by just do it on 2023/12/4.
//

#ifndef TINYRPC_ENDIAN_H
#define TINYRPC_ENDIAN_H
#include <endian.h>
#include <type_traits>

namespace tinyRPC {
    template<typename T>
    std::enable_if_t<sizeof(T) == 1, T> HostToNetwork(const T& val) { return val; }
    template<typename T>
    std::enable_if_t<sizeof(T) == 2, T> HostToNetwork(const T& val) { return htobe16(val); }
    template<typename T>
    std::enable_if_t<sizeof(T) == 4, T> HostToNetwork(const T& val) { return htobe32(val); }
    template<typename T>
    std::enable_if_t<sizeof(T) == 8, T> HostToNetwork(const T& val) { return htobe64(val); }
    template<typename T>
    std::enable_if_t<sizeof(T) == 1, T> NetworkToHost(const T& val) { return val; }
    template<typename T>
    std::enable_if_t<sizeof(T) == 2, T> NetworkToHost(const T& val) { return be16toh(val); }
    template<typename T>
    std::enable_if_t<sizeof(T) == 4, T> NetworkToHost(const T& val) { return be32toh(val); }
    template<typename T>
    std::enable_if_t<sizeof(T) == 8, T> NetworkToHost(const T& val) { return be64toh(val); }
}

#endif //TINYRPC_ENDIAN_H
