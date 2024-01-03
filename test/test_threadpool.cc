//
// Created by just do it on 2024/1/2.
//
#include "tinyRPC/exec/executor.h"
#include <iostream>

using namespace tinyRPC;
using namespace std::chrono_literals;

std::atomic_int cnt = 0;

int main() {
    ThreadPoolExecutor executor(10, 50, 300, 5000ms, ThreadPoolExecutor::RejectPolicy::ABORT);
    for(int i = 0; i < 200; i++) {
        executor.Execute([]() -> std::shared_ptr<RpcResponse> {
                cnt++;
                std::this_thread::sleep_for(1s);
                return nullptr;
            }, [](std::shared_ptr<RpcResponse>) {});
    }

    std::this_thread::sleep_for(4000ms);
    for(int i = 0; i < 100; i++) {
        executor.Execute([]() -> std::shared_ptr<RpcResponse> {
            cnt++;
            std::this_thread::sleep_for(1s);
            return nullptr;
        }, [](std::shared_ptr<RpcResponse>) {});
    }

    std::this_thread::sleep_for(5000ms);
    for(int i = 0; i < 150; i++) {
        executor.Execute([]() -> std::shared_ptr<RpcResponse> {
            cnt++;
            std::this_thread::sleep_for(1s);
            return nullptr;
        }, [](std::shared_ptr<RpcResponse>) {});
    }

    std::this_thread::sleep_for(10s);

    executor.Shutdown();
    std::cout << cnt << std::endl;

}