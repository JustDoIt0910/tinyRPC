//
// Created by just do it on 2023/12/10.
//
#include "services.pb.h"
#include "client/channel.h"
#include "rpc/controller.h"

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