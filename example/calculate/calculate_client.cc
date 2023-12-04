//
// Created by just do it on 2023/12/4.
//
#include "client/channel.h"
#include "protocol/controller.h"
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
}