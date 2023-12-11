//
// Created by just do it on 2023/12/10.
//
#include "test_service.pb.h"
#include "tinyRPC/client/channel.h"
#include "tinyRPC/rpc/controller.h"

int main() {
    tinyRPC::Channel channel("localhost", 9999);
    Greeter_Stub stub(&channel);
    tinyRPC::Controller controller;
    controller.SetTimeout(2000);

    HelloRequest req;
    req.set_name("zr");

    HelloReply reply;

    stub.SayHello(&controller, &req, &reply, nullptr);
    std::cout << reply.message() << std::endl;
}