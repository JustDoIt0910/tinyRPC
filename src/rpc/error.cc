//
// Created by just do it on 2023/12/6.
//
#include "tinyRPC/rpc/error.h"

namespace tinyRPC {

    rpc_error::error_code::error_code(error err, std::string detail): error_(err) {
        if(err >= messages_.size()) { throw std::out_of_range("Invalid error code"); }
        if(detail.length() > 0) {
            detail_ = messages_[err] + ": " + std::move(detail);
        }
        else { detail_ = messages_[err]; }
    }

    rpc_error::error_code::error rpc_error::error_code::code() const { return error_; }

    bool rpc_error::error_code::ok() const { return error_ == RPC_SUCCESS; }

    const char* rpc_error::error_code::message() const { return detail_.c_str(); }

    rpc_error::rpc_error(const error_code& ec): std::runtime_error(ec.message()), ec_(ec) {
        message_.append(std::runtime_error::what());
    }

    rpc_error::error_code& rpc_error::error() { return ec_; }

    const char* rpc_error::what() { return message_.c_str(); }

    std::vector<std::string> rpc_error::error_code::messages_{
            "Ok", "Invalid RPC method name or url", "No such service",
            "No such method", "Failed to parse RPC data",
            "Exception raised in procedure call", "Failed to serialize data",
            "Failed to parse data", "HTTP method not allowed",
            "Bad HTTP request", "Content-Length is required",
            "Unsupported media type", "Unsupported HTTP version",
            "Request rejected by thread pool", ""
    };


    connect_error::connect_error(std::error_code ec): std::system_error(ec) {
        message_.append(std::system_error::what());
    }

    const char* connect_error::what() const noexcept { return message_.c_str(); }

    timeout_error::timeout_error(const std::string& detail): std::runtime_error(detail) {
        message_.append(std::runtime_error::what());
    }

    const char* timeout_error::what() const noexcept { return message_.c_str(); }

    connect_timeout::connect_timeout(const std::string& server_addr, uint16_t port):
    timeout_error(detail(server_addr, port)) {}

    std::string connect_timeout::detail(const std::string& server_addr, uint16_t port) {
        std::stringstream ss;
        ss << "Connect [" << server_addr << ":" << std::to_string(port) << "] timeout";
        return ss.str();
    }

    no_endpoint_error::no_endpoint_error(const std::string &service):
    std::runtime_error("no endpoint for service " + service) {
        message_.append(std::runtime_error::what());
    }

    const char* no_endpoint_error::what() const noexcept { return message_.c_str(); }

    etcd_error::etcd_error(const std::string& detail): std::runtime_error(detail) {
        message_.append(std::runtime_error::what());
    }

    const char* etcd_error::what() const noexcept { return message_.c_str(); }

}