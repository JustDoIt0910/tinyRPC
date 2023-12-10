//
// Created by just do it on 2023/12/2.
//
#include "router/protobuf_router.h"
#include <google/protobuf/message.h>
#include "../example/calculate/calculate_service.pb.h"

using namespace google;

namespace tinyRPC {

    bool ProtobufRpcRouter::ParseServiceMethod(const std::string &full_name,
                                               std::string& service,
                                               std::string& method) const {
        size_t dot = full_name.find('.');
        if(dot == std::string::npos) { return false; }
        service = full_name.substr(0, dot);
        method = full_name.substr(dot + 1);
        return true;
    }

    bool ProtobufRpcRouter::ParseRequestData(google::protobuf::Message* message,
                                             const std::string &data) const {
        return message->ParseFromString(data);
    }

    bool ProtobufRpcRouter::SerializeResponseData(const google::protobuf::Message *message,
                                                  std::string *data) const {
        return message->SerializeToString(data);
    }

}