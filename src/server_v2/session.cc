//
// Created by just do it on 2024/1/1.
//
#include "tinyRPC/server_v2//session.h"
#include "tinyRPC/server_v2//server.h"
#include "tinyRPC/codec/protobuf_codec.h"
#include "tinyRPC/router/base_router.h"
#include <iostream>

namespace tinyRPC {

    Session::Session(Server* server, io_context& ioc, std::unique_ptr<Codec>& codec, Router* router):
    server_(server),
    ioc_(ioc),
    socket_(ioc),
    codec_(std::move(codec)),
    router_(router),
    closing_(false) {}

    void Session::Start() {
        char buf[50] = {0};
        auto ep = socket_.remote_endpoint();
        std::string addr_str = ep.address().to_string();
        sprintf(buf, "%s:%u", addr_str.c_str(), ep.port());
        id_ = std::string(buf);
        DoRead();
    }

    ip::tcp::socket& Session::Socket() { return socket_; }

    std::string& Session::Id() { return id_; }

    void Session::DoRead() {
        auto self = shared_from_this();
        socket_.async_read_some(codec_->Buffer(), [this, self] (std::error_code ec, std::size_t length) {
            if(closing_.load()) {  return; }
            if(!ec) {
                codec_->Consume(length);
                RpcRequest request;
                Codec::DecodeResult res;
                while((res = codec_->Next(request)) == Codec::DecodeResult::SUCCESS) {
                    RpcResponse resp = router_->Route(request);
                    std::string data = codec_->Encode(resp);
                    write_queue_.push(std::move(data));
                    if(write_queue_.size() == 1) { DoWrite(); }
                }
                if(res == Codec::DecodeResult::FATAL) {
                    std::string error_response = codec_->GetErrorResponse(request.msg_id_);
                    async_write(socket_, buffer(error_response.data(), error_response.length()),
                                [](std::error_code, size_t){});
                    Close();
                    return;
                }
                DoRead();
            }
            else if(ec == error::eof || ec == error::connection_reset) { Close(); }
        });
    }

    void Session::DoWrite() {
        if(closing_.load()) { return; }
        std::string& data = write_queue_.front();
        auto self = shared_from_this();
        async_write(socket_, buffer(data.data(), data.length()),
                    [this, self] (std::error_code ec, size_t transferred) {
                        if(closing_.load()) { return; }
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

    void Session::Close() {
        closing_.store(true);
        std::error_code e;
        e = socket_.shutdown(ip::tcp::socket::shutdown_both, e);
        if(e) {}
        socket_.close();
        server_->RemoveSession(id_);
    }

}