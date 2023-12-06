//
// Created by just do it on 2023/12/4.
//
#include "client/channel.h"
#include "rpc/controller.h"
#include "calculate_service.pb.h"
#include <vector>
#include <thread>
#include <atomic>


int main() {

    std::atomic_int cnt{0};
    std::atomic_bool go{false};

    std::vector<std::thread> threads;
    std::vector<std::shared_ptr<tinyRPC::Channel>> channels;
    channels.resize(200);
    for(int i = 0; i < 200; i++) {
        channels[i] = std::make_shared<tinyRPC::Channel>("127.0.0.1", 9999);
    }

    for(int i = 0; i < 200; i++) {
        threads.emplace_back([&cnt, &channels, i, &go](){
            CalculateService_Stub stub(channels[i].get());
            tinyRPC::Controller controller;
            controller.SetTimeout(2000);
            while(!go);
            for(int j = 0; j < 100; j++) {
                AddQuery query;
                query.set_a(10);
                query.set_b(20);

                AddResponse resp;
                stub.Add(&controller, &query, &resp, nullptr);
                if(resp.result() == 30){
                    cnt++;
                }
            }

        });
    }

    auto start = std::chrono::steady_clock::now();
    go.store(true);
    for(auto& t: threads){ t.join(); }
    auto end = std::chrono::steady_clock::now();
    double t = std::chrono::duration<double, std::milli>(end - start).count();
    double s = t / 1000.0;
    std::cout << 20000 / s << "calls/second" << std::endl;
    std::cout << cnt << std::endl;

}