//
// Created by just do it on 2023/12/7.
//
#include "bench.h"
#include "calculate_service.pb.h"

using namespace tinyRPC;


int main() {
    Bench bench(&CalculateService_Stub::Add, "localhost", 9999, 4);

    bench.SetQueryInitializer([](AddQuery* addQuery) {
        addQuery->set_a(100);
        addQuery->set_b(500);
    });

    bench.Start(5000, 500000);
}

