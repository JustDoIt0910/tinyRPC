//
// Created by just do it on 2023/12/1.
//

#ifndef TINYRPC_SESSION_H
#define TINYRPC_SESSION_H
#include "asio.hpp"
#include "tinyRPC/rpc/error.h"
#include <queue>
#include <atomic>

using namespace asio;

namespace tinyRPC {
    class Server;
    class Codec;
    class Router;
    class ThreadPoolExecutor;
    struct RpcResponse;

class Session: public std::enable_shared_from_this<Session>{
    public:
        Session(Server* server, io_context& ioc, std::unique_ptr<Codec>& codec,
                Router* router, ThreadPoolExecutor* executor);

        void Start();

        ip::tcp::socket& Socket();

        std::string& Id();

    private:
        void DoRead();

        void DoWrite();

        void Close();

        friend class Router;
        void RpcCallback(std::shared_ptr<RpcResponse> response);

        Server* server_;
        io_context& ioc_;
        std::unique_ptr<Codec> codec_;
        Router* router_;
        ThreadPoolExecutor* executor_;
        ip::tcp::socket socket_;
        std::queue<std::string> write_queue_;
        std::string id_;
        std::atomic_bool closing_;
    };

}

#endif //TINYRPC_SESSION_H
