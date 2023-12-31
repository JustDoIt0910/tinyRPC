//
// Created by just do it on 2023/12/4.
//
#include "tinyRPC/client/channel.h"
#include "tinyRPC/rpc/controller.h"
#include "calculate_service.pb.h"


int main() {

    tinyRPC::Channel channel("localhost", 9999);
    CalculateService_Stub stub(&channel);
    tinyRPC::Controller controller;

    AddQuery query;
    query.set_a(10);
    query.set_b(20);
    AddResponse resp;

    stub.Add(&controller, &query, &resp, nullptr);
    std::cout << resp.result() << std::endl;
}