//
// Created by just do it on 2023/12/5.
//
#include "tinyRPC/server_v2/server.h"
#include "calculate_service.pb.h"

using namespace std::chrono_literals;

class CalculateServiceImpl: public CalculateService {
public:
    void Add(::google::protobuf::RpcController* controller,
             const ::AddQuery* request,
             ::AddResponse* response,
             ::google::protobuf::Closure* done) override {
        response->set_result(request->a() + request->b());
        std::this_thread::sleep_for(10s);
        if(done) { done->Run(); }
    }
};

int main() {
    tinyRPC::Server server(9999);
    server.SetWorkerNum(8);
    server.RegisterService(std::make_shared<CalculateServiceImpl>(), true);
    server.Serve();
}