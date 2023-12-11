# tinyRPC
使用 asio 和 protobuf 的 RPC framework 最初版本

### 依赖

- tinyRPC 依赖 protobuf 进行序列化，使用版本为 3.19.4
- libuuid
  ```shell
  sudo apt-get install uuid-dev

### 编译

```shell
mkdir build && cd build
cmake ..
make
```
### 安装

```shell
sudo cmake --install .
```
### examples

- example/calculate: 简单的计算 rpc
- example/http: 使用 http gateway 同时提供 http 接口

注： example/http 在 tinyRPC 安装之前是编译不过的，所以要安装之后再打开注释

example/calculate/calculate_service.proto:
```protobuf
syntax = "proto3";
option cc_generic_services = true;


message AddQuery {
    int32 a = 1;
    int32 b = 2;
}

message AddResponse {
    int32 result= 1;
}

service CalculateService {
    rpc Add(AddQuery) returns (AddResponse);
}
```
生成 calculate_service.pb.h 和 calculate_service.pb.cc:
```shell
protoc --cpp_out=. calculate_service.proto
```
example/calculate/server.cc:
```c++
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
```

example/calculate/client.cc:
```c++
#include "tinyRPC/client/channel.h"
#include "tinyRPC/rpc/controller.h"
#include "calculate_service.pb.h"

int main() {

    tinyRPC::Channel channel("localhost", 9999);
    CalculateService_Stub stub(&channel);
    tinyRPC::Controller controller;

    AddQuery query;
    query.set_a(10);
    query.set_b(20);
    AddResponse resp;

    stub.Add(&controller, &query, &resp, nullptr);
    std::cout << resp.result() << std::endl;
}
```

tinyRPC还支持 http gateway 来提供 http 接口，只需在 proto 文件的 method 中定义映射成的 http 接口方法和 url:
example/http/test_service.proto:
```protobuf
syntax = "proto3";
option cc_generic_services = true;

import "tinyRPC/api/http_rule.proto";

message HelloRequest {
    string name = 1;
}

message HelloReply {
    string message = 1;
}

message GetUserRequest {
    string uid = 1;
}

enum Gender {
    GENDER_MALE = 0;
    GENDER_FEMALE = 1;
};

message GetUserReply {
    string username = 1;
    Gender gender = 2;
    string birth = 3;
}

service Greeter {
    rpc SayHello (HelloRequest) returns (HelloReply) {
        option (tinyRPC.api.http) = {
            method: "post",
            url: "/greet_service/greet"
        };
    }
}

service UserService {
    rpc GetUserById(GetUserRequest) returns (GetUserReply) {
        option (tinyRPC.api.http) = {
            method: "get",
            url: "/user_service/user"
        };
    }
}
```
在照常生成 .pb.h 和 .pb.cc 后，使用刚刚编译出的 protoc 插件生成 .gw.h 文件:
```shell
protoc --http-gw_out=. test_service.proto
```
就可以使用 HttpApiGateway 同时提供 rpc 和 http 服务了:
example/http/server.cc:
```c++
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
```
```shell
curl "http://192.168.40.129:8080/greet_service/greet" -H "Content-Type: application/json" -X POST -d "{\"name\": \"zr\"}"

# {"message":"Hello zr!"}
```
```shell
l "http://192.168.40.129:8080/user_service/user" -H "Content-Type: application/json" -X GET -d "{\"uid\": \"12345678\"}"

# {"username":"Zhou Huimin","gender":"GENDER_FEMALE","birth":"1967-11-10"}
```

如果 http method 或 url 与 proto 中定义的不匹配，会响应错误:
```shell
curl "http://192.168.40.129:8080/user_service/user" -H "Content-Type: application/json" -X POST -d "{\"uid\": \"20321331\"}"

# {"error code": 1,"message": Invalid RPC method name or url: post /user_service/user}
```

目前 Content-Type 仅支持 application/json

### benchmark

测试代码：benchmark/bench.cc	使用 2 个 I/O 线程，计算完成 50W 次 a + b service 请求的 qps

测试机器：Ubuntu 20.04 VMware 虚拟机，3G 内存，8 CPU

测试程序与服务器在同一机器

| QPS            | 500 conn | 1000 conn | 2000 conn | 5000 conn | 10000 conn |
| -------------- | -------- | --------- | --------- | --------- | ---------- |
| 1 I/O thread   | 24866    | 23316     | 24103     | 23329     | 22154      |
| 4 I/O threads  | 107419   | 103155    | 101532    | 95383     | 94173      |
| 8 I/O threads  | 111301   | 106188    | 101375    | 98493     | 95154      |
| 16 I/O threads | 114075   | 112186    | 106667    | 100096    | 98630      |

QPS 在 10W 左右，当前使用的 asio 线程模型是多线程共用同一 io_context，会导致竞争，达不到最优性能，后续考虑使用 io_context per thread 模型，并加入线程池。
