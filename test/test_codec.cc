//
// Created by just do it on 2023/12/5.
//
#include "rpc/codec.h"
#include <iostream>
#include <random>

using namespace tinyRPC;

int main() {
    Codec* codec = new ProtobufRpcCodec();

    RpcRequest req("test_id1", "CalculateService.Add", "fake protobuf data");
    std::string buf = codec->Encode(req);
    req = RpcRequest("test_id2", "CalculateService.Add", "fake protobuf data");
    buf.append(codec->Encode(req));
    req = RpcRequest("test_id3", "CalculateService.Add", "fake protobuf data");
    buf.append(codec->Encode(req));
    req = RpcRequest("test_id4", "CalculateService.Add", "fake protobuf data");
    buf.append(codec->Encode(req));
    req = RpcRequest("test_id5", "CalculateService.Add", "fake protobuf data");
    buf.append(codec->Encode(req));
    req = RpcRequest("test_id6", "CalculateService.Add", "fake protobuf data");
    buf.append(codec->Encode(req));
    req = RpcRequest("test_id7", "CalculateService.Add", "fake protobuf data");
    buf.append(codec->Encode(req));

    printf("buf len = %zu\n", buf.length());


    RpcRequest req_decoded;
    Codec::DecodeResult res;

    std::vector<int> divide;
    auto e = std::default_random_engine(time(nullptr));
    auto u = std::uniform_int_distribution<int>(1, buf.length() - 1);
    for(int i = 0; i < 10 + 1; i++) {
        divide.push_back(u(e));
    }
    divide.erase(divide.begin());
    std::sort(divide.begin(), divide.end());
    int p = 0;
    for(int i = 0; i < divide.size() + 1; i++) {
        int d = i < divide.size() ? divide[i] : buf.length();
        size_t len = d - p;
        printf("------incoming data [%zu] bytes------\n", len);
        memcpy(codec->Buffer().data(), buf.data() + p, len);
        p = d;

        codec->Consume(len);
        while(codec->Next(req_decoded) == Codec::DecodeResult::SUCCESS) {
            std::cout << req_decoded.msg_id_ << std::endl;
            std::cout << req_decoded.full_method_name_ << std::endl;
            std::cout << req_decoded.data_ << std::endl;
            std::cout << std::endl;
        }
    }
}