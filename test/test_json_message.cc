//
// Created by just do it on 2023/12/7.
//
#include <google/protobuf/util/json_util.h>
#include "../example/calculate/calculate_service.pb.h"
#include <iostream>

using namespace google::protobuf;

int main() {
    AddQuery query;
    query.set_a(10);
    query.set_b(30);
    std::string json;
    util::Status status = util::MessageToJsonString(query, &json);
    if(status.ok()) {
        std::cout << json << std::endl;
    }

    // reflection test
//    Message* message = &query;
//    auto desc = message->GetDescriptor();
//    auto field = desc->FindFieldByName("a");
//    if(field) {
//        auto refl = message->GetReflection();
//        refl->SetInt32(message, field, 123);
//    }
//
//    std::cout << query.a() << std::endl;
}