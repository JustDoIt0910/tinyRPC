//
// Created by just do it on 2023/12/9.
//

#ifndef TINYRPC_HTTP_ROUTER_H
#define TINYRPC_HTTP_ROUTER_H
#include "router/base_router.h"
#include <unordered_map>

namespace tinyRPC {

    class HttpProtobufMapper {
    public:
        bool Map(const std::string& http_method, const std::string& http_url,
                 std::string& protobuf_service, std::string& protobuf_method) {
            auto k = std::make_pair(http_method, http_url);
            auto it = mappings_.find(k);
            if(it == mappings_.end()) { return false; }
            protobuf_service = it->second.first;
            protobuf_method = it->second.second;
            return true;
        }

        HttpProtobufMapper() = default;

        HttpProtobufMapper(HttpProtobufMapper&&) = default;

        HttpProtobufMapper& operator=(HttpProtobufMapper&&) = default;

        virtual ~HttpProtobufMapper() = default;

    protected:
        struct HashPair {
            size_t operator()(const std::pair<std::string, std::string>& p) const {
                return std::hash<std::string>()(p.first) ^ std::hash<std::string>()(p.second);
            }
        };

        std::unordered_map<std::pair<std::string, std::string>,
                std::pair<std::string, std::string>, HashPair> mappings_;
    };

    class HttpRouter: public Router {
    public:
        explicit HttpRouter(std::unique_ptr<HttpProtobufMapper>& mapper): mapper_(std::move(mapper)) {}

        bool ParseServiceMethod(const std::string& full_name,
                                        std::string& service,
                                        std::string& method) const override;

        bool ParseRequestData(google::protobuf::Message* message,
                                      const std::string& data) const override;

        bool SerializeResponseData(const google::protobuf::Message* message,
                                           std::string* data) const override;

    private:
        std::unique_ptr<HttpProtobufMapper> mapper_;
    };

}

#endif //TINYRPC_HTTP_ROUTER_H
