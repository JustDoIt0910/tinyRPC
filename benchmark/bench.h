//
// Created by just do it on 2023/12/7.
//

#ifndef TINYRPC_BENCH_H
#define TINYRPC_BENCH_H
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <unordered_map>
#include "asio.hpp"

using namespace asio;

namespace tinyRPC {

    class Connection;

    class BenchClient {
    public:
        BenchClient(const std::string& addr, uint16_t port, int io_threads = 0);

        void Start(int num_connections, int num_request);

        ~BenchClient();

    private:
        std::string server_addr_;
        uint16_t port_;
        std::vector<std::thread> io_threads_;
        std::vector<io_context> io_contexts_;
        std::unordered_map<std::string, std::shared_ptr<Connection>> conns_;
    };

}

#endif //TINYRPC_BENCH_H
