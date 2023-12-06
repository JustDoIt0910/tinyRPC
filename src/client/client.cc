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
            auto promise = std::make_shared<CallPromise>();
            std::future<RpcResponse> fu = promise->get_future();
            SendRequest(std::move(data), request.msg_id_, promise);
            return fu;
        }

    private:
        using CallPromise = std::promise<RpcResponse>;
        using CallPromisePtr = std::shared_ptr<CallPromise>;

        void Connect(ip::tcp::resolver::results_type& endpoints) {
            connect_res_ = connect_promise_.get_future();
            async_connect(socket_, endpoints,
                          [this](std::error_code ec, const tcp::endpoint&) {
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

        void SendRequest(std::string data, const std::string& id, CallPromisePtr promise) {
            ioc_.post([this, d = std::move(data), id, p = std::move(promise)] () mutable {
                pending_calls_[id] = p;
                write_queue_.push(std::move(d));
                if(write_queue_.size() == 1) { DoWrite(); }
            });
        }

        void DoRead() {
            socket_.async_read_some(codec_->Buffer(),
                                    [this](std::error_code ec, std::size_t length) {
                if(!ec) {
                    codec_->Consume(length);
                    RpcResponse resp;
                    while(codec_->Next(resp) == Codec::DecodeResult::SUCCESS) {
                        auto pending_call = pending_calls_.find(resp.msg_id_);
                        if(pending_call == pending_calls_.end()) { continue; }
                        if(resp.ec_ == rpc_error::error_code::RPC_SUCCESS) {
                            pending_call->second->set_value(std::move(resp));
                        }
                        else {
                            rpc_error::error_code rpc_ec(static_cast<rpc_error::error_code::error>(resp.ec_),
                                                         resp.error_detail_);
                            rpc_error error(rpc_ec);
                            pending_call->second->set_exception(std::make_exception_ptr(error));
                        }
                    }
                    DoRead();
                }
                else {

                }
            });
        }

        void DoWrite() {
            std::string& data = write_queue_.front();
            async_write(socket_, buffer(data.data(), data.length()),
                        [this] (std::error_code ec, size_t transferred) {
                if(!ec) {
                    write_queue_.pop();
                    if(!write_queue_.empty()) { DoWrite(); }
                }
                else {
                    // TODO handle write error
                    std::cout << ec.message() << std::endl;
                }
            });
        }

        io_context ioc_;
        tcp::socket socket_;
        std::thread io_thread_;
        std::unique_ptr<Codec> codec_;
        std::promise<void> connect_promise_;
        std::future<void> connect_res_;
        std::queue<std::string> write_queue_;
        std::unordered_map<std::string, CallPromisePtr> pending_calls_;
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