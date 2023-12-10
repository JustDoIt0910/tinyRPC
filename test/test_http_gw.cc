//
// Created by just do it on 2023/12/9.
//
#include "http_api_gw.h"
#include "calculate_service.pb.h"

using namespace tinyRPC;

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

    Server server("127.0.0.1", 9999);
    HttpApiGateway gw(&server, 8080);

    server.RegisterService(std::make_shared<CalculateServiceImpl>());

    server.Serve();
}