//
// Created by just do it on 2023/12/5.
//
#ifndef USE_SERVER_V1
#include "tinyRPC/server_v2/server.h"
#else
#include "tinyRPC/server/server.h"
#endif
#include "calculate_service.pb.h"

using namespace std::chrono_literals;

class CalculateServiceImpl: public CalculateService {
public:
    void Add(::google::protobuf::RpcController* controller,
             const ::AddQuery* request,
             ::AddResponse* response,
             ::google::protobuf::Closure* done) override {
        response->set_result(request->a() + request->b());
        if(done) { done->Run(); }
    }
};

int main() {
    tinyRPC::Server server("config.yaml");
    server.RegisterService(std::make_shared<CalculateServiceImpl>(), true);
    server.Serve();
}