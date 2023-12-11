//
// Created by just do it on 2023/12/9.
//
#include "tinyRPC/router/http_router.h"
#include "tinyRPC/comm/string_util.h"
#include <google/protobuf/util/json_util.h>

namespace tinyRPC {

    bool HttpRouter::ParseServiceMethod(const std::string &full_name, std::string &service,
                                        std::string &method) const {
        std::vector<std::string> v;
        StringUtil::Split(full_name, " ", v);
        return mapper_->Map(v[0], v[1], service, method);
    }

    bool HttpRouter::ParseRequestData(google::protobuf::Message *message, const std::string &data) const {
        return google::protobuf::util::JsonStringToMessage(data.c_str(), message).ok();
    }

    bool HttpRouter::SerializeResponseData(const google::protobuf::Message *message, std::string *data) const {
        return google::protobuf::util::MessageToJsonString(*message, data).ok();
    }

}