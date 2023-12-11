//
// Created by just do it on 2023/12/7.
//

#ifndef TINYRPC_BENCH_H
#define TINYRPC_BENCH_H
#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <thread>
#include <unordered_map>
#include "method_traits.h"
#include "countdown_latch.h"
#include "tinyRPC/client/channel.h"
#include "tinyRPC/rpc/closure.h"
#include "tinyRPC/rpc/controller.h"
#include "asio.hpp"

using namespace asio;

namespace tinyRPC {

    template<typename RpcMethod>
    class Bench {
    public:
        using ServiceStub = service_type<RpcMethod>;

        using ResponseType = response_type<RpcMethod>;

        using QueryType = query_type<RpcMethod>;

        Bench(RpcMethod method, std::string addr, uint16_t port, unsigned io_threads = 0):
        method_(method),
        server_addr_(std::move(addr)),
        port_(port),
        io_contexts_(GetThreadNum(io_threads)) {}

        void SetQueryInitializer(std::function<void(QueryType*)> initializer) {
            query_initializer_ = std::move(initializer);
        }

        void Start(int num_connections, int num_request) {
            finish_latch_ = std::make_shared<CountdownLatch>(num_connections);
            int req_per_conn = num_request / num_connections;
            int req_last = num_request - req_per_conn * (num_connections - 1);
            int ioc_idx = 0;
            for(int i = 1; i <= num_connections; i++) {
                int req_num = (i < num_connections) ? req_per_conn : req_last;
                auto conn = std::make_shared<Connection>(&io_contexts_[ioc_idx], this, server_addr_, port_, req_num);
                connections_.push_back(conn);
                ioc_idx++;
                if(ioc_idx == io_contexts_.size()) { ioc_idx = 0; }
            }
            for(auto& ioc: io_contexts_) {
                io_threads_.emplace_back([&ioc] () { ioc.run(); });
            }
            for(auto& conn: connections_) { conn->WaitConnectDone(); }
            std::cout << "connect done" << std::endl;
            auto start = std::chrono::steady_clock::now();
            for(auto& conn: connections_) { conn->Start(); }
            finish_latch_->Wait();
            auto end = std::chrono::steady_clock::now();
            double ms = std::chrono::duration<double, std::milli>(end - start).count();
            std::cout << num_request << " query complete. using " << ms << "ms" << std::endl;
            double s = ms / 1000.0;
            std::cout << "qps = " << num_request / s << " query/second" << std::endl;
        }

        ~Bench() {
            for(auto& ioc: io_contexts_) { ioc.stop(); }
            for(auto& thread: io_threads_) { thread.join(); }
        }

    private:
        struct Connection {
        public:
            Connection(io_context* ctx, Bench* bench, const std::string& addr, uint16_t port, int request_num):
                    channel_(ctx, addr, port),
                    stub_(&channel_),
                    request_num_(request_num),
                    bench_(bench) {
                controller_.SetAsync(true);
            }

            void HandleCallFinish(rpc_error::error_code ec, google::protobuf::Message*) {
                if(ec.ok()) {
                    request_num_--;
                    if(request_num_ == 0) {
                        bench_->finish_latch_->CountDown();
                        return;
                    }
                    NewRequest();
                }
                else {
                    std::cout << "rpc failed: " << ec.message() << std::endl;
                }
            }

            void WaitConnectDone() { channel_.WaitForConnect(); }

            void Start() { NewRequest(); }

        private:
            void NewRequest() {
                QueryType query;
                bench_->query_initializer_(&query);
                auto closure = MakeRpcClosure<ResponseType>([this] (rpc_error::error_code ec, google::protobuf::Message* message) {
                    HandleCallFinish(std::move(ec), message);
                });
                (stub_.*(bench_->method_))(&controller_, &query, nullptr, closure);
            }

            Channel channel_;
            ServiceStub stub_;
            Controller controller_;
            int request_num_;
            Bench* bench_;
        };

        unsigned GetThreadNum(unsigned io_threads) {
            unsigned threads = (!io_threads) ? std::thread::hardware_concurrency() : io_threads;
            if(!threads) { threads = 2; }
            return threads;
        }

        RpcMethod method_;
        std::string server_addr_;
        uint16_t port_;
        std::vector<io_context> io_contexts_;
        std::vector<std::thread> io_threads_;
        std::function<void(QueryType*)> query_initializer_;
        std::vector<std::shared_ptr<Connection>> connections_;
        std::shared_ptr<CountdownLatch> finish_latch_;
    };

}

#endif //TINYRPC_BENCH_H
