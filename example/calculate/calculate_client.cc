//
// Created by just do it on 2023/12/4.
//
#include "client/channel.h"
#include "rpc/controller.h"
#include "rpc/closure.h"
#include "calculate_service.pb.h"
#include <vector>
#include <thread>
#include <atomic>

//void HandleResponse(tinyRPC::rpc_error::error_code ec, google::protobuf::Message* response) {
//    if(ec.ok()) {
//        auto* resp = dynamic_cast<AddResponse*>(response);
//        std::cout << resp->result() << std::endl;
//    }
//    else {
//        std::cout << ec.message() << std::endl;
//    }
//}

int main() {

    std::atomic_int cnt{0};
    std::atomic_int msg_id{1};

    std::vector<std::thread> threads;
    std::vector<std::shared_ptr<tinyRPC::Channel>> channels;
    channels.resize(300);
    for(int i = 0; i < 300; i++) {
        channels[i] = std::make_shared<tinyRPC::Channel>("127.0.0.1", 9999);
    }

    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < 300; i++) {
        threads.emplace_back([&cnt, &channels, i, &msg_id](){
            CalculateService_Stub stub(channels[i].get());
            tinyRPC::Controller controller;
            controller.SetTimeout(2000);
            for(int j = 0; j < 300; j++) {
                AddQuery query;
                query.set_a(10);
                query.set_b(20);

                AddResponse resp;
                controller.SetRequestId(std::to_string(msg_id.fetch_add(1)));
                stub.Add(&controller, &query, &resp, nullptr);
                if(resp.result() == 30){
                    cnt++;
                }
            }

        });
    }

    for(auto& t: threads){ t.join(); }
    auto end = std::chrono::steady_clock::now();
    double t = std::chrono::duration<double, std::milli>(end - start).count();
    double s = t / 1000.0;
    std::cout << 90000 / s << " calls/second" << std::endl;
    std::cout << cnt << std::endl;

}