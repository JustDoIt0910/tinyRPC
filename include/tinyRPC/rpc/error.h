//
// Created by just do it on 2023/12/2.
//

#ifndef TINYRPC_ERROR_H
#define TINYRPC_ERROR_H
#include <system_error>
#include <sstream>
#include <utility>
#include <vector>

namespace tinyRPC {

    class rpc_error: public std::runtime_error {
    public:
        class error_code {
        public:
            enum error {
                RPC_SUCCESS,
                RPC_INVALID_METHOD_NAME,
                RPC_NO_SUCH_SERVICE,
                RPC_NO_SUCH_METHOD,
                RPC_BAD_DATA,
                RPC_CALL_ERROR,
                RPC_SERIALIZE_ERROR,
                RPC_PARSE_ERROR,
                RPC_HTTP_METHOD_NOT_ALLOWED,
                RPC_HTTP_BAD_REQUEST,
                RPC_HTTP_LENGTH_REQUIRED,
                RPC_HTTP_UNSUPPORTED_MEDIA_TYPE,
                RPC_HTTP_VERSION_NOT_SUPPORTED,
                RPC_THREAD_POOL_REJECTED,
                RPC_IN_PROGRESS
            };

            explicit error_code(error err = RPC_SUCCESS, std::string detail = "");

            error code() const;

            bool ok() const;

            const char* message() const;

        private:
            error error_;
            std::string detail_;
            static std::vector<std::string> messages_;
        };

        explicit rpc_error(const error_code& ec);

        error_code& error();

        const char* what();

    private:
        error_code ec_;
        std::string message_{"tinyRPC::rpc_error: "};
    };

    class connect_error: public std::system_error {
    public:
        explicit connect_error(std::error_code ec);

        const char* what() const noexcept override;

    private:
        std::string message_{"tinyRPC::connect_error: "};
    };

    class timeout_error: public std::runtime_error {
    public:
        explicit timeout_error(const std::string& detail);

        const char* what() const noexcept override;

    private:
        std::string message_{"tinyRPC::timeout_error: "};
    };

    class connect_timeout: public timeout_error {
    public:
        connect_timeout(const std::string& server_addr, uint16_t port);

    private:
        std::string static detail(const std::string& server_addr, uint16_t port);
    };

    class no_endpoint_error: public std::runtime_error {
    public:
        explicit no_endpoint_error(const std::string& service);

        const char* what() const noexcept override;

    private:
        std::string message_{"tinyRPC::no_endpoint_error: "};
    };

    class etcd_error: public std::runtime_error {
    public:
        explicit etcd_error(const std::string& detail);

        const char* what() const noexcept override;

    private:
        std::string message_{"tinyRPC::etcd_error: "};
    };

    class reject_policy_error: public std::runtime_error {
    public:
        explicit reject_policy_error(const std::string& detail);

        const char* what() const noexcept override;

    private:
        std::string message_{"tinyRPC::reject_policy_error: "};
    };

}

#endif //TINYRPC_ERROR_H
