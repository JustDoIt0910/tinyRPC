//
// Created by just do it on 2023/12/1.
//
#include "server/session.h"
#include "server/server.h"
#include "codec/protobuf_codec.h"
#include "router/base_router.h"
#include <cstdio>
#include <iostream>

namespace tinyRPC {

    Session::Session(Server* server, io_context& ioc, ip::tcp::socket sock,
                     std::unique_ptr<Codec>& codec, Router* router):
    server_(server),
    ioc_(ioc),
    codec_(std::move(codec)),
    router_(router),
    socket_(std::move(sock)),
    read_strand_(ioc_),
    write_strand_(ioc_),
    closing_(false) {}

    void Session::Start() {
        char buf[50] = {0};
        auto ep = socket_.remote_endpoint();
        std::string addr_str = ep.address().to_string();
        sprintf(buf, "%s:%u", addr_str.c_str(), ep.port());
        id_ = std::string(buf);
        DoRead();
    }

    std::string& Session::Id() { return id_; }

    void Session::DoRead() {
        auto self = shared_from_this();
        socket_.async_read_some(codec_->Buffer(), read_strand_.wrap([this, self](std::error_code ec,
                std::size_t length) {
            if(closing_.load()) {  return; }
            if(!ec) {
                codec_->Consume(length);
                RpcRequest request;
                while(codec_->Next(request) == Codec::DecodeResult::SUCCESS) {
                    ioc_.post([this, self, req = std::move(request)] () {
                        RpcResponse resp = router_->Route(req);
                        std::string data = codec_->Encode(resp);
                        write_strand_.post([this, self, d = std::move(data)] () mutable {
                            write_queue_.push(std::move(d));
                            if(write_queue_.size() == 1) { DoWrite(); }
                        });
                    });
                }
                DoRead();
            }
            else if(ec == error::eof || ec == error::connection_reset) {
                closing_.store(true);
                write_strand_.post([this, self] () {
                    std::error_code e;
                    e = socket_.shutdown(ip::tcp::socket::shutdown_both, e);
                    if(e) {

                    }
                    socket_.close();
                    server_->RemoveSession(id_);
                });
            }
        }));
    }

    void Session::DoWrite() {
        if(closing_.load()) { return; }
        std::string& data = write_queue_.front();
        auto self = shared_from_this();
        async_write(socket_, buffer(data.data(), data.length()),
                    write_strand_.wrap([this, self] (std::error_code ec, size_t transferred) {
                        if(closing_.load()) { return; }
                        if(!ec) {
                            write_queue_.pop();
                            if(!write_queue_.empty()) { DoWrite(); }
                        }
                        else {
                            // TODO handle write error
                            std::cout << ec.message() << std::endl;
                        }
                    }));
    }
}