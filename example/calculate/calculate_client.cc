//
// Created by just do it on 2023/12/4.
//
#include "client/channel.h"
#include "rpc/controller.h"
#include "calculate_service.pb.h"

int main() {
    tinyRPC::Channel channel("127.0.0.1", 9999);
    CalculateService_Stub stub(&channel);
    tinyRPC::Controller controller;
    controller.SetTimeout(2000);

    AddQuery query;
    query.set_a(10);
    query.set_b(20);

    AddResponse resp;

    stub.Add(&controller, &query, &resp, nullptr);
}