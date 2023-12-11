#include <iostream>
#include "test_service.gw.h"
#include "test_service.pb.h"

using namespace tinyRPC;

class GreeterServiceImpl: public Greeter {
    void SayHello(google::protobuf::RpcController* controller,
                  const ::HelloRequest* request,
                  ::HelloReply* response,
                  ::google::protobuf::Closure* done) override {
        response->set_message("Hello " + request->name() + "!");
        if(done) { done->Run(); }
    }
};

class UserServiceImpl: public UserService {
    void GetUserById(google::protobuf::RpcController* controller,
                     const ::GetUserRequest* request,
                     ::GetUserReply* response,
                     ::google::protobuf::Closure* done) override {
        if(request->uid() == "20321331") {
            response->set_username("JustDoIt0910");
            response->set_gender(GENDER_MALE);
            response->set_birth("2001-09-10");
        }
        else if(request->uid() == "12345678") {
            response->set_username("Zhou Huimin");
            response->set_gender(GENDER_FEMALE);
            response->set_birth("1967-11-10");
        }
        else {
            throw std::runtime_error("User not found.");
        }
        if(done) { done->Run(); }
    }
};

int main() {
    Server server(9999);
    HttpApiGateway gw(&server, 8080);

    server.RegisterService(std::make_shared<GreeterServiceImpl>());
    server.RegisterService(std::make_shared<UserServiceImpl>());

    server.Serve();
}
