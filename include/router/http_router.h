//
// Created by just do it on 2023/12/9.
//

#ifndef TINYRPC_HTTP_ROUTER_H
#define TINYRPC_HTTP_ROUTER_H
#include "router/base_router.h"

namespace tinyRPC {

    class HttpProtobufMapper {
    public:
        virtual bool Map(const std::string& http_method, const std::string& http_url,
                         std::string& protobuf_service, std::string& protobuf_method) = 0;

        virtual ~HttpProtobufMapper() = default;
    };

    class HttpRouter: public Router {
        HttpRouter(std::unique_ptr<HttpProtobufMapper>& mapper): mapper_(std::move(mapper)) {}

        virtual bool ParseServiceMethod(const std::string& full_name,
                                        std::string& service,
                                        std::string& method) const;

        virtual bool ParseRequestData(google::protobuf::Message* message,
                                      const std::string& data) const;

        virtual bool SerializeResponseData(const google::protobuf::Message* message,
                                           std::string* data) const;

    private:
        std::unique_ptr<HttpProtobufMapper> mapper_;
    };

}

#endif //TINYRPC_HTTP_ROUTER_H
