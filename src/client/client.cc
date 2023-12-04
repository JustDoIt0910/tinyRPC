//
// Created by just do it on 2023/12/3.
//
#include "client/client.h"
#include "rpc/codec.h"
#include "asio.hpp"
#include <iostream>

using namespace asio;
using namespace asio::ip;

namespace tinyRPC {

    class Client::Impl {
    public:
        Impl(const std::string &address, uint16_t port): socket_(ioc_) {
            codec_ = std::make_unique<ProtobufRpcCodec>();
            tcp::resolver resolver(ioc_);
            auto endpoints = resolver.resolve(address, std::to_string(port));
            Connect(endpoints);
        }

        void Start() { io_thread_ = std::thread([this] () { ioc_.run(); }); }

        void Stop() {
            ioc_.stop();
            io_thread_.join();
        }

        std::future<RpcResponse> Call(const RpcRequest& request) {
            std::string data = codec_->Encode(request);
            std::future<RpcResponse> fu;
            return fu;
        }

    private:
        void Connect(ip::tcp::resolver::results_type& endpoints) {
            async_connect(socket_, endpoints, [this](std::error_code ec, const tcp::endpoint&) {
                if(!ec) {

                }
                else {

                }
            });
        }

        io_context ioc_;
        tcp::socket socket_;
        std::thread io_thread_;
        std::unique_ptr<Codec> codec_;
    };

    Client::Client(const std::string &address, uint16_t port) {
        pimpl_ = std::make_unique<Impl>(address, port);
        pimpl_->Start();
    }

    std::future<RpcResponse> Client::Call(const RpcRequest &request) { return pimpl_->Call(request); }

    Client::~Client() { pimpl_->Stop(); }

}