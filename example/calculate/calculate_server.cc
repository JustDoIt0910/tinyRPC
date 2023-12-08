//
// Created by just do it on 2023/12/5.
//
#include "server/server.h"
#include "calculate_service.pb.h"
#include <atomic>

class CalculateServiceImpl: public CalculateService {
public:
    std::atomic<int> cnt{0};
    void Add(::google::protobuf::RpcController* controller,
             const ::AddQuery* request,
             ::AddResponse* response,
             ::google::protobuf::Closure* done) override {
        response->set_result(request->a() + request->b());
        cnt++;
        if(cnt == 500000) {std::cout << "500000" << std::endl;}
        if(done) { done->Run(); }
    }
};

int main() {
    tinyRPC::Server server(9999);
    server.SetWorkerNum(5);
    server.RegisterService(std::make_shared<CalculateServiceImpl>());
    server.Serve();
}