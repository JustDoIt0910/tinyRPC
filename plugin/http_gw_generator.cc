//
// Created by just do it on 2023/12/10.
//
#include "http_rule.pb.h"
#include "comm/string_util.h"

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
                          "#include \"server/server.h\"\n"
                          "#include \"router/http_router.h\"\n"
                          "#include \"codec/http_codec.h\"\n"
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
                          "        explicit HttpApiGateway(Server* server, uint16_t port = 8080):\n"
                          "        server_(server),\n"
                          "        acceptor_(server->GetAcceptor()) {\n"
                          "            server->SetGateway(this);\n"
                          "            asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), port);\n"
                          "            acceptor_.open(ep.protocol());\n"
                          "            acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));\n"
                          "            acceptor_.bind(ep);\n"
                          "            acceptor_.listen();\n"
                          "            std::unique_ptr<HttpProtobufMapper> mapper = std::make_unique<HttpProtobufMapperImpl>();\n"
                          "            router_ = std::make_unique<HttpRouter>(mapper);\n"
                          "            StartAccept();\n"
                          "        }\n"
                          "\n"
                          "        void RegisterService(std::shared_ptr<google::protobuf::Service> service) override {\n"
                          "            router_->RegisterService(service);\n"
                          "        }\n"
                          "\n"
                          "    private:\n"
                          "        void StartAccept() {\n"
                          "            acceptor_.async_accept([this] (std::error_code ec, asio::ip::tcp::socket socket) {\n"
                          "                std::unique_ptr<Codec> codec = std::make_unique<HttpRpcCodec>();\n"
                          "                server_->NewSession(socket, codec, router_.get());\n"
                          "                StartAccept();\n"
                          "            });\n"
                          "        }\n"
                          "\n"
                          "        Server* server_;\n"
                          "        asio::ip::tcp::acceptor acceptor_;\n"
                          "        std::unique_ptr<HttpRouter> router_;\n"
                          "    };\n"
                          "\n"
                          "}\n"
                          "\n"
                          "#endif //TINYRPC_HTTP_API_GW_H";

class MyGenerator : public google::protobuf::compiler::cpp::CppGenerator {
    bool Generate(const FileDescriptor* file, const std::string& parameter,
                  GeneratorContext* generator_context,
                  std::string* error) const override {
        auto stream = generator_context->Open("http_api_gw.h");
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