//
// Created by just do it on 2023/12/5.
//
#include "tinyRPC/codec/protobuf_codec.h"
#include <iostream>
#include <random>

using namespace tinyRPC;

int main() {
//    Codec* codec = new ProtobufRpcCodec();

//    RpcRequest req("test_id1", "CalculateService.Add", "fake protobuf data");
//    std::string buf = codec->Encode(req);
//    req = RpcRequest("test_id2", "CalculateService.Add", "fake protobuf data");
//    buf.append(codec->Encode(req));
//    req = RpcRequest("test_id3", "CalculateService.Add", "fake protobuf data");
//    buf.append(codec->Encode(req));
//    req = RpcRequest("test_id4", "CalculateService.Add", "fake protobuf data");
//    buf.append(codec->Encode(req));
//    req = RpcRequest("test_id5", "CalculateService.Add", "fake protobuf data");
//    buf.append(codec->Encode(req));
//    req = RpcRequest("test_id6", "CalculateService.Add", "fake protobuf data");
//    buf.append(codec->Encode(req));
//    req = RpcRequest("test_id7", "CalculateService.Add", "fake protobuf data");
//    buf.append(codec->Encode(req));
//
//    printf("buf len = %zu\n", buf.length());
//
//
//    RpcRequest req_decoded;
//    Codec::DecodeResult res;
//
//    std::vector<int> divide;
//    auto e = std::default_random_engine(time(nullptr));
//    auto u = std::uniform_int_distribution<int>(1, buf.length() - 1);
//    for(int i = 0; i < 10 + 1; i++) {
//        divide.push_back(u(e));
//    }
//    divide.erase(divide.begin());
//    std::sort(divide.begin(), divide.end());
//    int p = 0;
//    for(int i = 0; i < divide.size() + 1; i++) {
//        int d = i < divide.size() ? divide[i] : buf.length();
//        size_t len = d - p;
//        printf("------incoming data [%zu] bytes------\n", len);
//        memcpy(codec->Buffer().data(), buf.data() + p, len);
//        p = d;
//
//        codec->Consume(len);
//        while(codec->Next(req_decoded) == Codec::DecodeResult::SUCCESS) {
//            std::cout << req_decoded.msg_id_ << std::endl;
//            std::cout << req_decoded.full_method_name_ << std::endl;
//            std::cout << req_decoded.data_ << std::endl;
//            std::cout << std::endl;
//        }
//    }


//    RpcResponse resp("test_id1", 0, "Ok", "fake protobuf data");
//    std::string buf = codec->Encode(resp);
//    resp = RpcResponse("test_id2", 0, "Ok", "fake protobuf data");
//    buf.append(codec->Encode(resp));
//    resp = RpcResponse("test_id3", 0, "Ok", "fake protobuf data");
//    buf.append(codec->Encode(resp));
//    resp = RpcResponse("test_id4", 0, "Ok", "fake protobuf data");
//    buf.append(codec->Encode(resp));
//    resp = RpcResponse("test_id5", 0, "Ok", "fake protobuf data");
//    buf.append(codec->Encode(resp));
//    resp = RpcResponse("test_id6", 0, "Ok", "fake protobuf data");
//    buf.append(codec->Encode(resp));
//    resp = RpcResponse("test_id7", 0, "Ok", "fake protobuf data");
//    buf.append(codec->Encode(resp));
//
//    printf("buf len = %zu\n", buf.length());
//
//
//    RpcResponse resp_decoded;
//    Codec::DecodeResult res;
//
//    std::vector<int> divide;
//    auto e = std::default_random_engine(time(nullptr));
//    auto u = std::uniform_int_distribution<int>(1, buf.length() - 1);
//    for(int i = 0; i < 10 + 1; i++) {
//        divide.push_back(u(e));
//    }
//    divide.erase(divide.begin());
//    std::sort(divide.begin(), divide.end());
//    int p = 0;
//    for(int i = 0; i < divide.size() + 1; i++) {
//        int d = i < divide.size() ? divide[i] : buf.length();
//        size_t len = d - p;
//        printf("------incoming data [%zu] bytes------\n", len);
//        memcpy(codec->Buffer().data(), buf.data() + p, len);
//        p = d;
//
//        codec->Consume(len);
//        while(codec->Next(resp_decoded) == Codec::DecodeResult::SUCCESS) {
//            std::cout << resp_decoded.msg_id_ << std::endl;
//            std::cout << resp_decoded.error_detail_ << std::endl;
//            std::cout << resp_decoded.data_ << std::endl;
//            std::cout << std::endl;
//        }
//    }

//    HttpRpcCodec* codec = new HttpRpcCodec;
//    char data[] = "aaaaaaa\r\nbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\r\ncc\r\nd\r\neeeeeeeee\r\nffffffffffffffff"
//                  "ffff\r\ngg\r\nhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
//                  "hhhhh\r\ni\r\n\r\n";
//
//    std::vector<int> divide;
//    auto e = std::default_random_engine(time(nullptr));
//    auto u = std::uniform_int_distribution<int>(1, strlen(data) - 1);
//    for(int i = 0; i < 10 + 1; i++) {
//        divide.push_back(u(e));
//    }
//    divide.erase(divide.begin());
//    std::sort(divide.begin(), divide.end());
//    int p = 0;
//    for(int i = 0; i < divide.size() + 1; i++) {
//        int d = i < divide.size() ? divide[i] : strlen(data);
//        size_t len = d - p;
//        printf("------incoming data [%zu] bytes------\n", len);
//        memcpy(codec->Buffer().data(), data + p, len);
//        p = d;
//
//        codec->Consume(len);
//        std::string line;
//        while(codec->NextLine(line)) {
//            std::cout << line << std::endl;
//        }
//    }
}