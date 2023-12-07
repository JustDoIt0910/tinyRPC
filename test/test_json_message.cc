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
}