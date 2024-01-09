//
// Created by just do it on 2023/12/4.
//
#include "tinyRPC/client/channel.h"
#include "tinyRPC/rpc/controller.h"
#include "calculate_service.pb.h"


int main() {

    //tinyRPC::Channel channel("localhost", 9999);
    tinyRPC::Channel channel("http://110.40.210.125:2379",
                             std::make_shared<tinyRPC::RoundRobinBalancer>());
    CalculateService_Stub stub(&channel);
    tinyRPC::Controller controller;

    AddQuery query;
    query.set_a(10);
    query.set_b(20);
    AddResponse resp;

    stub.Add(&controller, &query, &resp, nullptr);
    std::cout << resp.result() << std::endl;
    stub.Add(&controller, &query, &resp, nullptr);
    std::cout << resp.result() << std::endl;
    stub.Add(&controller, &query, &resp, nullptr);
    std::cout << resp.result() << std::endl;
    stub.Add(&controller, &query, &resp, nullptr);
    std::cout << resp.result() << std::endl;
}