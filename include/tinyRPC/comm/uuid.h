//
// Created by just do it on 2023/12/2.
//

#ifndef TINYRPC_UUID_H
#define TINYRPC_UUID_H

#include <uuid/uuid.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <iostream>

class UUID {
public:
    UUID() { memset(uuid_, 0, sizeof(uuid_)); };

    explicit UUID(uuid_t uuid) { uuid_copy(uuid_, uuid); }

    UUID(const UUID& uuid) { uuid_copy(uuid_, uuid.uuid_); }

    UUID& operator=(const UUID& uuid) {
        uuid_copy(uuid_, uuid.uuid_);
        return *this;
    }

    bool operator==(const UUID& uuid) const {
        return uuid_compare(uuid_, uuid.uuid_) == 0;
    }

    static UUID generate() {
        uuid_t uuid;
        uuid_generate(uuid);
        return UUID(uuid);
    }

    bool is_null() { return uuid_is_null(uuid_); }

    void clear() { uuid_clear(uuid_); }

    std::string to_string(const std::string& split = "-", bool upper = true) const {
        std::ostringstream oss;
        if(upper) oss << std::uppercase;
        else oss << std::nouppercase;
        oss << std::hex << std::setfill('0')
            << std::setw(2) << static_cast<uint16_t>(uuid_[0])
            << std::setw(2) << static_cast<uint16_t>(uuid_[1])
            << std::setw(2) << static_cast<uint16_t>(uuid_[2])
            << std::setw(2) << static_cast<uint16_t>(uuid_[3])
            << split
            << std::setw(2) << static_cast<uint16_t>(uuid_[4])
            << std::setw(2) << static_cast<uint16_t>(uuid_[5])
            << split
            << std::setw(2) << static_cast<uint16_t>(uuid_[6])
            << std::setw(2) << static_cast<uint16_t>(uuid_[7])
            << split
            << std::setw(2) << static_cast<uint16_t>(uuid_[8])
            << std::setw(2) << static_cast<uint16_t>(uuid_[9])
            << split
            << std::setw(2) << static_cast<uint16_t>(uuid_[10])
            << std::setw(2) << static_cast<uint16_t>(uuid_[11])
            << std::setw(2) << static_cast<uint16_t>(uuid_[12])
            << std::setw(2) << static_cast<uint16_t>(uuid_[13])
            << std::setw(2) << static_cast<uint16_t>(uuid_[14])
            << std::setw(2) << static_cast<uint16_t>(uuid_[15]);
        return oss.str();
    }

    bool from_string(const std::string& uuid_str) {
        if(uuid_parse(uuid_str.c_str(), uuid_) < 0) {
            std::cerr << "Invalid UUID format. UUID must be %08x-%04x-%04x-%04x-%012x in \"printf()\" format,"
                         " 36 bytes plus the tailing \\0." << std::endl;
            return false;
        }
        return true;
    }

private:
    uuid_t uuid_{};
};

inline std::ostream& operator<<(std::ostream& os, const UUID& uuid) {
    os << uuid.to_string();
    return os;
}

#endif //TINYRPC_UUID_H
