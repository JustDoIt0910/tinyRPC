//
// Created by just do it on 2023/12/1.
//
#include "server/session.h"
#include "rpc/codec.h"
#include "rpc/abstract_router.h"
#include <cstdio>
#include <iostream>

namespace tinyRPC {

    Session::Session(io_context& ioc, std::unique_ptr<Codec>& codec, Router* router):
    ioc_(ioc),
    codec_(std::move(codec)),
    router_(router),
    socket_(ioc_),
    read_strand_(ioc_),
    write_strand_(ioc_) {}

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
        socket_.async_read_some(codec_->Buffer(), read_strand_.wrap([this, self](std::error_code ec,
                std::size_t length) {
            if(!ec) {
                codec_->Consume(length);
                RpcRequest request;
                while(codec_->Next(request) == Codec::DecodeResult::SUCCESS) {
                    ioc_.post([this, self, req = std::move(request)] () {
                        RpcResponse resp = router_->Route(req);
                    });
                }
            }
            else {
                // TODO handler read error
            }
        }));
    }
}