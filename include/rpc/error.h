//
// Created by just do it on 2023/12/2.
//

#ifndef TINYRPC_ERROR_H
#define TINYRPC_ERROR_H
#include <system_error>
#include <sstream>

namespace tinyRPC {
    enum class ErrorCode {

    };

    class connect_error: public std::system_error {
    public:
        explicit connect_error(std::error_code ec): std::system_error(ec) {
            message_.append(std::system_error::what());
        }
        const char* what() const noexcept override { return message_.data(); }

    private:
        std::string message_{"tinyRPC::connect_error: "};
    };

    class timeout_error: public std::runtime_error {
    public:
        explicit timeout_error(const std::string& detail): std::runtime_error(detail) {
            message_.append(std::runtime_error::what());
        }
        const char* what() const noexcept override { return message_.data(); }

    private:
        std::string message_{"tinyRPC::timeout_error: "};
    };

    class connect_timeout: public timeout_error {
    public:
        connect_timeout(const std::string& server_addr, uint16_t port):
                timeout_error(detail(server_addr, port)) {}

    private:
        std::string static detail(const std::string& server_addr, uint16_t port) {
            std::stringstream ss;
            ss << "Connect [" << server_addr << ":" << std::to_string(port) << "] timeout";
            return ss.str();
        }
    };
}

#endif //TINYRPC_ERROR_H
