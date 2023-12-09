//
// Created by just do it on 2023/12/7.
//
#include "tool/bench.h"
#include "calculate_service.pb.h"

using namespace tinyRPC;


int main() {
    Bench bench(&CalculateService_Stub::Add, "localhost", 9999, 4);

    bench.SetQueryInitializer([](AddQuery* addQuery) {
        addQuery->set_a(100);
        addQuery->set_b(500);
    });

    bench.Start(2000, 500000);
}

