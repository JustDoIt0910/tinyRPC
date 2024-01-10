#ifndef TINYRPC_HTTP_API_GW_H
#define TINYRPC_HTTP_API_GW_H
#include "tinyRPC/server_v2/server.h"
#include "tinyRPC/server_v2/gw.h"
#include "tinyRPC/router/http_router.h"

namespace tinyRPC {

    class HttpProtobufMapperImpl: public HttpProtobufMapper {
    public:
        HttpProtobufMapperImpl(): HttpProtobufMapper() {
            mappings_ = {{{"post", "/greet_service/greet"},{"Greeter", "SayHello"}},{{"get", "/user_service/user"},{"UserService", "GetUserById"}}};
        }
    };

    class HttpApiGateway: public AbstractHttpApiGateway {
    public:
        HttpApiGateway(Server* server, uint16_t port):
        AbstractHttpApiGateway(HttpProtobufMapper::New<HttpProtobufMapperImpl>(), server, port) {}
    };

}

#endif //TINYRPC_HTTP_API_GW_H