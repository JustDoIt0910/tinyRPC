//
// Created by just do it on 2023/12/5.
//
#include "tinyRPC/server/server.h"
#include "calculate_service.pb.h"

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
    tinyRPC::Server server(9999);
    server.SetWorkerNum(8);
    server.RegisterService(std::make_shared<CalculateServiceImpl>());
    server.Serve();
}