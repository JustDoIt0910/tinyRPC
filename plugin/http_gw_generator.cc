//
// Created by just do it on 2023/12/10.
//
#include "tinyRPC/api/http_rule.pb.h"
#include "tinyRPC/comm/string_util.h"

#include "google/protobuf/compiler/cpp/cpp_generator.h"
#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/printer.h"

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace google::protobuf::compiler;

static std::string code = "#ifndef TINYRPC_HTTP_API_GW_H\n"
                          "#define TINYRPC_HTTP_API_GW_H\n"
                          "#include \"tinyRPC/server/server.h\"\n"
                          "#include \"tinyRPC/server/gw.h\"\n"
                          "#include \"tinyRPC/router/http_router.h\"\n"
                          "\n"
                          "namespace tinyRPC {\n"
                          "\n"
                          "    class HttpProtobufMapperImpl: public HttpProtobufMapper {\n"
                          "    public:\n"
                          "        HttpProtobufMapperImpl(): HttpProtobufMapper() {\n"
                          "            mappings_ = {[http_protobuf_map]};\n"
                          "        }\n"
                          "    };\n"
                          "\n"
                          "    class HttpApiGateway: public AbstractHttpApiGateway {\n"
                          "    public:\n"
                          "        HttpApiGateway(Server* server, uint16_t port):\n"
                          "        AbstractHttpApiGateway(HttpProtobufMapper::New<HttpProtobufMapperImpl>(), server, port) {}\n"
                          "    };\n"
                          "\n"
                          "}\n"
                          "\n"
                          "#endif //TINYRPC_HTTP_API_GW_H";

class MyGenerator : public google::protobuf::compiler::cpp::CppGenerator {
    bool Generate(const FileDescriptor* file, const std::string& parameter,
                  GeneratorContext* generator_context,
                  std::string* error) const override {
        std::vector<std::string> v;
        tinyRPC::StringUtil::Split(file->name(), ".", v);
        auto stream = generator_context->Open(v[0] + ".gw.h");
        Printer printer(stream, ' ');

//        std::vector<std::pair<std::string, std::string>> params;
//        ParseGeneratorParameter(parameter, &params);

        std::string http_protobuf_map;

        for(int s = 0; s < file->service_count(); s++) {
            const ServiceDescriptor* service = file->service(s);
            for(int m = 0; m < service->method_count(); m++) {
                const MethodDescriptor* method = service->method(m);
                const MethodOptions& options = method->options();
                auto r = options.GetExtension(tinyRPC::api::http);
                auto p = std::make_pair(r.method(), r.url());
                std::string http_rule;
                tinyRPC::StringUtil::ToLower(p.first);
                tinyRPC::StringUtil::PairToString(p, http_rule);
                p = std::make_pair(service->name(), method->name());
                std::string protobuf_service;
                tinyRPC::StringUtil::PairToString(p, protobuf_service);
                std::string map;
                map.push_back('{');
                map.append(http_rule);
                map.push_back(',');
                map.append(protobuf_service);
                map.push_back('}');
                http_protobuf_map.append(map);
                http_protobuf_map.push_back(',');
            }
        }
        http_protobuf_map.erase(http_protobuf_map.size() - 1);
        code.replace(code.find("[http_protobuf_map]"), strlen("[http_protobuf_map]"), http_protobuf_map);
        printer.PrintRaw(code);
        return true;
    }
};

int main(int argc, char* argv[]) {
    MyGenerator generator;
    return PluginMain(argc, argv, &generator);
}