//
// Created by just do it on 2023/12/3.
//
#include "client/client.h"
#include "rpc/codec.h"
#include "asio.hpp"
#include <iostream>
#include <queue>

using namespace asio;
using namespace asio::ip;

namespace tinyRPC {

    class Client::Impl {
    public:
        Impl(const std::string &address, uint16_t port):
        socket_(ioc_), server_addr_(address), port_(port) {
            codec_ = std::make_unique<ProtobufRpcCodec>();
            tcp::resolver resolver(ioc_);
            auto endpoints = resolver.resolve(address, std::to_string(port));
            Connect(endpoints);
        }

        void Start() { io_thread_ = std::thread([this] () { ioc_.run(); }); }

        void SetConnectTimeout(int seconds) {
            connect_timeout_ = std::chrono::milliseconds(seconds);
        }

        void Stop() {
            ioc_.stop();
            io_thread_.join();
        }

        std::future<RpcResponse> Call(const RpcRequest& request) {
            if(connect_timeout_) {
                auto status = connect_res_.wait_for(*connect_timeout_);
                if(status == std::future_status::ready) { connect_res_.get(); }
                else { throw connect_timeout(server_addr_, port_); }
            }
            else { connect_res_.get(); }
            std::string data = codec_->Encode(request);
            Write(std::move(data));
            std::future<RpcResponse> fu;
            return fu;
        }

    private:
        void Connect(ip::tcp::resolver::results_type& endpoints) {
            connect_res_ = connect_promise_.get_future();
            async_connect(socket_, endpoints, [this](std::error_code ec, const tcp::endpoint&) {
                if(!ec) {
                    connect_promise_.set_value();
                    DoRead();
                }
                else {
                    connect_error error(ec);
                    connect_promise_.set_exception(std::make_exception_ptr(error));
                }
            });
        }

        void Write(std::string data) {
            write_queue_.push(std::move(data));
            if(write_queue_.empty()) {

            }
        }

        void DoRead() {

        }

        void DoWrite() {

        }

        io_context ioc_;
        tcp::socket socket_;
        std::thread io_thread_;
        std::unique_ptr<Codec> codec_;
        std::promise<void> connect_promise_;
        std::future<void> connect_res_;
        std::queue<std::string> write_queue_;
        std::optional<std::chrono::milliseconds> connect_timeout_;
        std::string server_addr_;
        uint16_t port_;
    };

    Client::Client(const std::string &address, uint16_t port) {
        pimpl_ = std::make_unique<Impl>(address, port);
        pimpl_->Start();
    }

    std::future<RpcResponse> Client::Call(const RpcRequest &request) { return pimpl_->Call(request); }

    void Client::SetConnectTimeout(int milli) { pimpl_->SetConnectTimeout(milli); }

    Client::~Client() { pimpl_->Stop(); }

}